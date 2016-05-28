namespace DDBotGui
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this._lbl1 = new System.Windows.Forms.Label();
            this.statusLbl = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.serialCommRB = new System.Windows.Forms.RadioButton();
            this.socketCommRb = new System.Windows.Forms.RadioButton();
            this.leftSpeedTb = new System.Windows.Forms.TrackBar();
            this.leftSpeedLbl = new System.Windows.Forms.Label();
            this.logTxt = new System.Windows.Forms.Label();
            this.dataLbl = new System.Windows.Forms.Label();
            this.rightSpeedLbl = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.rightSpeedTb = new System.Windows.Forms.TrackBar();
            this.label2 = new System.Windows.Forms.Label();
            this.stateLbl = new System.Windows.Forms.Label();
            this.stopBtn = new System.Windows.Forms.Button();
            this.manualBtn = new System.Windows.Forms.Button();
            this.restartBtn = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.leftSpeedTb)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.rightSpeedTb)).BeginInit();
            this.SuspendLayout();
            // 
            // _lbl1
            // 
            this._lbl1.AutoSize = true;
            this._lbl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._lbl1.Location = new System.Drawing.Point(150, 69);
            this._lbl1.Name = "_lbl1";
            this._lbl1.Size = new System.Drawing.Size(79, 25);
            this._lbl1.TabIndex = 0;
            this._lbl1.Text = "Status:";
            // 
            // statusLbl
            // 
            this.statusLbl.AutoSize = true;
            this.statusLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.statusLbl.Location = new System.Drawing.Point(235, 69);
            this.statusLbl.Name = "statusLbl";
            this.statusLbl.Size = new System.Drawing.Size(0, 25);
            this.statusLbl.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(150, 147);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(122, 25);
            this.label1.TabIndex = 0;
            this.label1.Text = "Left Speed:";
            // 
            // serialCommRB
            // 
            this.serialCommRB.AutoSize = true;
            this.serialCommRB.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F);
            this.serialCommRB.Location = new System.Drawing.Point(154, 25);
            this.serialCommRB.Name = "serialCommRB";
            this.serialCommRB.Size = new System.Drawing.Size(158, 29);
            this.serialCommRB.TabIndex = 2;
            this.serialCommRB.TabStop = true;
            this.serialCommRB.Text = "Serial COMM";
            this.serialCommRB.UseVisualStyleBackColor = true;
            this.serialCommRB.CheckedChanged += new System.EventHandler(this.commRbs_CheckedChanged);
            // 
            // socketCommRb
            // 
            this.socketCommRb.AutoSize = true;
            this.socketCommRb.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F);
            this.socketCommRb.Location = new System.Drawing.Point(318, 25);
            this.socketCommRb.Name = "socketCommRb";
            this.socketCommRb.Size = new System.Drawing.Size(169, 29);
            this.socketCommRb.TabIndex = 2;
            this.socketCommRb.TabStop = true;
            this.socketCommRb.Text = "Socket COMM";
            this.socketCommRb.UseVisualStyleBackColor = true;
            this.socketCommRb.CheckedChanged += new System.EventHandler(this.commRbs_CheckedChanged);
            // 
            // leftSpeedTb
            // 
            this.leftSpeedTb.Location = new System.Drawing.Point(273, 147);
            this.leftSpeedTb.Maximum = 101;
            this.leftSpeedTb.Minimum = -100;
            this.leftSpeedTb.Name = "leftSpeedTb";
            this.leftSpeedTb.Size = new System.Drawing.Size(384, 45);
            this.leftSpeedTb.TabIndex = 3;
            this.leftSpeedTb.Value = 25;
            this.leftSpeedTb.Scroll += new System.EventHandler(this.trackBar1_Scroll);
            this.leftSpeedTb.MouseDown += new System.Windows.Forms.MouseEventHandler(this.speedTbMouseDown);
            this.leftSpeedTb.MouseUp += new System.Windows.Forms.MouseEventHandler(this.speedTbMouseUp);
            // 
            // leftSpeedLbl
            // 
            this.leftSpeedLbl.AutoSize = true;
            this.leftSpeedLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.leftSpeedLbl.Location = new System.Drawing.Point(667, 147);
            this.leftSpeedLbl.Name = "leftSpeedLbl";
            this.leftSpeedLbl.Size = new System.Drawing.Size(0, 25);
            this.leftSpeedLbl.TabIndex = 0;
            // 
            // logTxt
            // 
            this.logTxt.AutoSize = true;
            this.logTxt.Location = new System.Drawing.Point(657, 13);
            this.logTxt.Name = "logTxt";
            this.logTxt.Size = new System.Drawing.Size(0, 13);
            this.logTxt.TabIndex = 4;
            // 
            // dataLbl
            // 
            this.dataLbl.AutoSize = true;
            this.dataLbl.Location = new System.Drawing.Point(152, 277);
            this.dataLbl.Name = "dataLbl";
            this.dataLbl.Size = new System.Drawing.Size(35, 13);
            this.dataLbl.TabIndex = 5;
            this.dataLbl.Text = "label2";
            // 
            // rightSpeedLbl
            // 
            this.rightSpeedLbl.AutoSize = true;
            this.rightSpeedLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.rightSpeedLbl.Location = new System.Drawing.Point(667, 195);
            this.rightSpeedLbl.Name = "rightSpeedLbl";
            this.rightSpeedLbl.Size = new System.Drawing.Size(0, 25);
            this.rightSpeedLbl.TabIndex = 0;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(150, 195);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(136, 25);
            this.label3.TabIndex = 0;
            this.label3.Text = "Right Speed:";
            // 
            // rightSpeedTb
            // 
            this.rightSpeedTb.Location = new System.Drawing.Point(273, 195);
            this.rightSpeedTb.Maximum = 101;
            this.rightSpeedTb.Minimum = -100;
            this.rightSpeedTb.Name = "rightSpeedTb";
            this.rightSpeedTb.Size = new System.Drawing.Size(384, 45);
            this.rightSpeedTb.TabIndex = 3;
            this.rightSpeedTb.Value = 25;
            this.rightSpeedTb.Scroll += new System.EventHandler(this.trackBar1_Scroll);
            this.rightSpeedTb.MouseDown += new System.Windows.Forms.MouseEventHandler(this.speedTbMouseDown);
            this.rightSpeedTb.MouseUp += new System.Windows.Forms.MouseEventHandler(this.speedTbMouseUp);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(149, 105);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(68, 25);
            this.label2.TabIndex = 0;
            this.label2.Text = "State:";
            // 
            // stateLbl
            // 
            this.stateLbl.AutoSize = true;
            this.stateLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.stateLbl.Location = new System.Drawing.Point(235, 105);
            this.stateLbl.Name = "stateLbl";
            this.stateLbl.Size = new System.Drawing.Size(0, 25);
            this.stateLbl.TabIndex = 1;
            // 
            // stopBtn
            // 
            this.stopBtn.BackColor = System.Drawing.Color.Red;
            this.stopBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 26.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.stopBtn.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.stopBtn.Location = new System.Drawing.Point(12, 12);
            this.stopBtn.Name = "stopBtn";
            this.stopBtn.Size = new System.Drawing.Size(128, 118);
            this.stopBtn.TabIndex = 6;
            this.stopBtn.Text = "STOP";
            this.stopBtn.UseVisualStyleBackColor = false;
            this.stopBtn.Click += new System.EventHandler(this.stopBtn_Click);
            // 
            // manualBtn
            // 
            this.manualBtn.BackColor = System.Drawing.Color.Orange;
            this.manualBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.manualBtn.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.manualBtn.Location = new System.Drawing.Point(12, 136);
            this.manualBtn.Name = "manualBtn";
            this.manualBtn.Size = new System.Drawing.Size(128, 62);
            this.manualBtn.TabIndex = 6;
            this.manualBtn.Text = "MANUAL";
            this.manualBtn.UseVisualStyleBackColor = false;
            this.manualBtn.Click += new System.EventHandler(this.manualBtn_Click);
            // 
            // restartBtn
            // 
            this.restartBtn.BackColor = System.Drawing.Color.Purple;
            this.restartBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.restartBtn.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.restartBtn.Location = new System.Drawing.Point(12, 204);
            this.restartBtn.Name = "restartBtn";
            this.restartBtn.Size = new System.Drawing.Size(128, 62);
            this.restartBtn.TabIndex = 6;
            this.restartBtn.Text = "RESTART";
            this.restartBtn.UseVisualStyleBackColor = false;
            this.restartBtn.Click += new System.EventHandler(this.restartBtn_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(975, 562);
            this.Controls.Add(this.restartBtn);
            this.Controls.Add(this.manualBtn);
            this.Controls.Add(this.stopBtn);
            this.Controls.Add(this.stateLbl);
            this.Controls.Add(this.statusLbl);
            this.Controls.Add(this.dataLbl);
            this.Controls.Add(this.logTxt);
            this.Controls.Add(this.rightSpeedTb);
            this.Controls.Add(this.leftSpeedTb);
            this.Controls.Add(this.socketCommRb);
            this.Controls.Add(this.serialCommRB);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.rightSpeedLbl);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.leftSpeedLbl);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._lbl1);
            this.Name = "Form1";
            this.Text = "DDBot GUI";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.leftSpeedTb)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.rightSpeedTb)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _lbl1;
        private System.Windows.Forms.Label statusLbl;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RadioButton serialCommRB;
        private System.Windows.Forms.RadioButton socketCommRb;
        private System.Windows.Forms.TrackBar leftSpeedTb;
        private System.Windows.Forms.Label leftSpeedLbl;
        private System.Windows.Forms.Label logTxt;
        private System.Windows.Forms.Label dataLbl;
        private System.Windows.Forms.Label rightSpeedLbl;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TrackBar rightSpeedTb;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label stateLbl;
        private System.Windows.Forms.Button stopBtn;
        private System.Windows.Forms.Button manualBtn;
        private System.Windows.Forms.Button restartBtn;
    }
}

