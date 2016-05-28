using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;

namespace DDBotGui
{
    enum Requests
    {
        SERIAL_REQUEST_LEFT_POWER,
        SERIAL_REQUEST_RIGHT_POWER,
        SERIAL_REQUEST_LEFT_DELTA,
        SERIAL_REQUEST_RIGHT_DELTA,
        SERIAL_UPDATE_LEFT_SPEED,
        SERIAL_UPDATE_RIGHT_SPEED,
        SERIAL_UPDATE_TARGET_SPEED,
        SERIAL_REQUEST_STATE,
        SERIAL_UPDATE_STATE
    };

    enum States
    {
        START,
        RUNNING,
        MANUAL,
        STOPPED
    }

    public partial class Form1 : Form
    {
        const string HELLO_MESSAGE = "HELO";
        TcpListener listener;
        TcpClient client;
        SerialPort currentPort;
        Thread updateThread;
        bool updateSpeed = true;
        bool isConnected = false;


        public Form1()
        {
            InitializeComponent();
            setIsConnected(false);
        }

        private void stopListening()
        {
            if (listener != null)
            {
                listener.Stop();
                listener = null;
            }
        }

        private void listenToConnection()
        {
            stopListening();
            listener = new TcpListener(System.Net.IPAddress.Any, 1338);
            listener.Start();
            listener.BeginAcceptTcpClient(new AsyncCallback(onSocketConnected), listener);

            setStatus("Listening to socket connection...", Color.Yellow);
        }

        private void onSocketConnected(IAsyncResult ar)
        {
            TcpListener currentListener = (TcpListener)ar.AsyncState;

            if (currentListener != listener)
            {
                log("Ignoring old listener");
                return;
            }

            try
            {
                client = listener.EndAcceptTcpClient(ar);
            }
            catch (Exception se)
            {
                log("ERROR: accept error: '" + se.Message + "'");
                return;
            }

            string hello = System.Text.Encoding.Default.GetString(ReadData(HELLO_MESSAGE.Length));

            if (hello != HELLO_MESSAGE)
            {
                log("ERROR: Invalid hello '" + hello + "'");
                setStatus("ERROR: Invalid hello", Color.Red);
            }
            else
            {
                setStatus("Connected to socket", Color.Green);
                setIsConnected(true);
                StartTimer();
            }

        }

        private void WriteData(byte[] data)
        {
            if (socketCommRb.Checked)
            {
                client.GetStream().Write(data, 0, data.Length);
            }
            else
            {
                currentPort.Write(data, 0, data.Length);
            }
        }

        private byte[] ReadData(int length)
        {
            if (socketCommRb.Checked)
            {
                byte[] data = new byte[length];
                int read = 0;
                while (read < length)
                {
                    read += client.GetStream().Read(data, read, length - read);
                }
                log(BitConverter.ToString(data));
                return data;
            }
            else
            {
                return StringToBytes(readBytesFromSerial(length));
            }
        }

        static byte[] StringToBytes(string str)
        {
            byte[] bytes = new byte[str.Length * sizeof(char)];
            System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }

        private void log(string line)
        {
            if (this.InvokeRequired)
            {
                this.Invoke((MethodInvoker)delegate
                { log(line); });
            }
            else
            {
                logTxt.Text = line + "\n" + logTxt.Text;
            }
        }

        private void SetComPort()
        {
            try
            {
                // string[] ports = SerialPort.GetPortNames();
                string[] ports = new string[] {"/dev/ttyACM0"};
                setStatus("Enumerating serial ports...", Color.Yellow);
                foreach (string port in ports)
                {
                    log("Trying " + port);
                    currentPort = new SerialPort(port, 9600);
                    if (DetectArduino())
                    {
                        setStatus("Connected to serial", Color.Green);
                        setIsConnected(true);
                        StartTimer();
                        return;
                    }
                }
                setStatus("No serial port found", Color.Red);
            }
            catch (Exception e)
            {
                setStatus("ERROR: " + e.Message, Color.Red);
            }
        }

        private string readBytesFromSerial(int bytes)
        {
            string returnMessage = "";
            while (bytes > 0)
            {
                int intReturnASCII = currentPort.ReadByte();
                returnMessage = returnMessage + Convert.ToChar(intReturnASCII);
                bytes--;
            }
            return returnMessage;
        }

        private bool DetectArduino()
        {
            try
            {
                currentPort.Encoding = System.Text.Encoding.GetEncoding(28591);
                currentPort.Open();
                int count = currentPort.BytesToRead;

                //ComPort.name = returnMessage;
                string helloMessage = readBytesFromSerial(HELLO_MESSAGE.Length);
                if (helloMessage == HELLO_MESSAGE)
                {
                    return true;
                }
                else
                {
                    currentPort.Close();
                    return false;
                }
            }
            catch (Exception e)
            {
                return false;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void setLeftTb(int value)
        {
            leftSpeedTb.Value = value;
            leftSpeedLbl.Text = value.ToString();
        }

        private void setRightTb(int value)
        {
            rightSpeedTb.Value = value;
            rightSpeedLbl.Text = value.ToString();
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            if (sender == leftSpeedTb)
            {
                leftSpeedLbl.Text = leftSpeedTb.Value.ToString();

                SendCommand(Requests.SERIAL_UPDATE_LEFT_SPEED, (byte)leftSpeedTb.Value);
            }

            if (sender == rightSpeedTb)
            {
                rightSpeedTb.Text = rightSpeedTb.Value.ToString();

                SendCommand(Requests.SERIAL_UPDATE_RIGHT_SPEED, (byte)rightSpeedTb.Value);
            }
        }

        private void commRbs_CheckedChanged(object sender, EventArgs e)
        {
            if (socketCommRb.Checked && sender == socketCommRb)
            {
                listenToConnection();
            }
            else if (serialCommRB.Checked && sender == serialCommRB)
            {
                SetComPort();
            }
        }


        private void setStatus(string text, Color color)
        {
            if (this.InvokeRequired)
            {
                this.Invoke((MethodInvoker)delegate
                        { setStatus(text, color); });
            }
            else
            {
                statusLbl.Text = text;
                statusLbl.ForeColor = color;
            }
        }

        private void SendCommand(Requests command, byte value)
        {
            WriteData(new byte[] { (byte)command, value });
        }

        private byte[] GetValue(Requests command, int resultLength)
        {
            SendCommand(command, 0);
            return ReadData(resultLength);
        }

        private byte GetByte(Requests command)
        {
            return GetValue(command, 1)[0];
        }

        private int GetUint32(Requests command)
        {
            return (int)BitConverter.ToUInt32(GetValue(command, 4), 0);
        }

        private void StartTimer()
        {
            if (updateThread == null)
            {
                updateThread = new Thread(new ThreadStart(timerTick));
                updateThread.Start();
            }
        }

        private void setIsConnected(bool value)
        {
            if (this.InvokeRequired)
            {
                this.Invoke((MethodInvoker)delegate
                { setIsConnected(value); });
            }
            else
            {
                isConnected = stopBtn.Enabled = manualBtn.Enabled
                    = restartBtn.Enabled = leftSpeedTb.Enabled = rightSpeedTb.Enabled = value;
            }

        }

        private void timerTick()
        {
            while (true)
            {

                try
                {
                    if (!isConnected)
                        continue;
                    byte state = GetByte(Requests.SERIAL_REQUEST_STATE);

                    byte leftPower = GetByte(Requests.SERIAL_REQUEST_LEFT_POWER);
                    byte rightPower = GetByte(Requests.SERIAL_REQUEST_RIGHT_POWER);

                    /*   int leftDelta = GetUint32(Requests.SERIAL_REQUEST_LEFT_DELTA);
                      // int rightDelta = GetUint32(Requests.SERIAL_REQUEST_RIGHT_DELTA)

                            this.Invoke((MethodInvoker)delegate
                           {
                               dataLbl.Text = "\n leftDelta   " + leftDelta;
                           });*/

                    //byte leftSpeed = GetByte(Requests.SERIAL_UPDATE_LEFT_SPEED);
                    int leftDelta = GetUint32(Requests.SERIAL_REQUEST_LEFT_DELTA);


                    //byte rightSpeed = GetByte(Requests.SERIAL_UPDATE_RIGHT_SPEED);
                    int rightDelta = GetUint32(Requests.SERIAL_REQUEST_RIGHT_DELTA);

                    this.Invoke((MethodInvoker)delegate
                        {
                            stateLbl.Text = ((States)state).ToString();

                            dataLbl.Text = "\n leftPower   " + leftPower +
                                      "\n leftDelta   " + leftDelta +
                                      "\n             " +
                                      "\n rightPower  " + rightPower +
                                      "\n rightDelta  " + rightDelta;

                            if (updateSpeed)
                            {
                                setLeftTb(leftPower);
                                setRightTb(rightPower);
                            }
                        });
                }
                catch (Exception ex)
                {
                    log("ERROR: " + ex.Message);
                    setIsConnected(false);
                    if (socketCommRb.Checked)
                    {
                        listenToConnection();
                    }
                    else if (serialCommRB.Checked)
                    {
                        SetComPort();
                    }
                }

                Thread.Sleep(40);
            }
        }

        private void speedTbMouseDown(object sender, MouseEventArgs e)
        {
            updateSpeed = false;
        }

        private void speedTbMouseUp(object sender, MouseEventArgs e)
        {
            updateSpeed = true;
        }

        private void stopBtn_Click(object sender, EventArgs e)
        {
            SendCommand(Requests.SERIAL_UPDATE_STATE, (byte)States.STOPPED);
            setLeftTb(0);
            setRightTb(0);
        }

        private void manualBtn_Click(object sender, EventArgs e)
        {
            SendCommand(Requests.SERIAL_UPDATE_STATE, (byte)States.MANUAL);
            setLeftTb(0);
            setRightTb(0);
        }

        private void restartBtn_Click(object sender, EventArgs e)
        {
            SendCommand(Requests.SERIAL_UPDATE_STATE, (byte)States.START);
        }

    }
}
