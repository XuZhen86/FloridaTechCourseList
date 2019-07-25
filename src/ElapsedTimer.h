#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#include<QElapsedTimer>
#include<QList>

class ElapsedTimer{
    public:
    ElapsedTimer(const QString &itemName);
    long long restart();
    static QList<QPair<QString,long long>> getAll();

    private:
    typedef struct ElapsedTimerAttribute_{
        ElapsedTimerAttribute_(const QString &itemName);

        QString itemName;
        QElapsedTimer elapsedTimer;
        long long lastValue;
    }ElapsedTimerAttribute;
    static QList<ElapsedTimerAttribute> elapsedTimerAttributes;

    private:
    QList<ElapsedTimerAttribute>::iterator attrIter;
};

#endif // ELAPSEDTIMER_H
