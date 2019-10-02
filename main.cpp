#include <QCoreApplication>
#include <stdlib.h>
#include <iostream>

#define COUNT 100
#define v_size 5
#define MARK '*'
#define AXIS '.'
#define SETVALUE '='

using namespace std;

class PID
{
public:
    PID();
    float run();
    bool isacceptable();
    vector<float> v1, v2;
    float setvalue;
private:
    float kp, ki, kd;
    float err, lasterr, interr;
    float value;
    float threshold;
};

class Map
{
public:
    Map(PID *pid);
    void plot();
    float maxValue();
private:
    PID *pid;
    float scale = 0.1;
};



PID::PID()
{
    kp = 0.5;
    ki = 0.3;
    kd = 0.1;
    err = 0.0;
    lasterr = 0.0;
    interr = 0;
    value = 0;
    setvalue = 100;
    threshold = 0.1;
    v1 = {setvalue, setvalue, setvalue, setvalue, setvalue};
    v2 = {};

}

float PID::run()
{
    err = setvalue - value;
    interr += err;
    value = kp * err + ki * interr + kd * (err - lasterr);
    lasterr = err;
    v2.push_back(value);

    return value;
}

bool PID::isacceptable()
{
    v1.push_back(err);
    v1.erase(v1.begin());

    for(float i = 0; i < v1.size(); i++)
    {
        if(fabs(v1[i]) > threshold)
        {
            return false;
        }
    }

    return true;
}

Map::Map(PID *pid)
{
    this->pid = pid;
}

void Map::plot()
{
    int length = pid->v2.size();
    int height = int(maxValue() * scale + 1) > int(scale * pid->setvalue + 1) ? int(maxValue() * scale + 1) : int(scale * pid->setvalue + 1);
    char array[height][length];

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < length; x++)
        {
            if(y == int(scale * pid->v2[x]))
                array[y][x] = MARK;
            else if(y == int(scale * pid->setvalue + 1) - 1)
                array[y][x] = SETVALUE;
            else
                array[y][x] = AXIS;
        }
    }

    for(int y = height - 1; y >= 0; y--)
    {
        for(int x = 0; x < length; x++)
            cout << array[y][x] << ' ';
        cout << endl;
    }


}

float Map::maxValue()
{
    float max = 0;
    for(float i = 0; i < pid->v2.size(); i++)
    {
        if(pid->v2[i] > max)
            max = pid->v2[i];
    }

    return max;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PID pid;
    Map map(&pid);

    for(int i = 0; i < COUNT; i++)
    {
        cout << i << "   " << pid.run() << endl;

        if(pid.isacceptable())
        {
            map.plot();
            return 0;
        }
    }

    cout << "PID is not constrained. Change steps or PID values." << endl;

    return a.exec();
}
