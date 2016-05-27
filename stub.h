void pinMode(int pin, int type);
int digitalRead(int pin);
void digitalWrite(int pin, int value);

#define LOW 0
#define HIGH 1

#define OUTPUT 0
#define INPUT 1

class Servo
{
public:
    void attach(int pin);
    void write(int value);
private:
    int pin;
};

long map(long x, long in_min, long in_max, long out_min, long out_max);

void delay(int milliseconds);
