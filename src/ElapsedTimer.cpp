#include"ElapsedTimer.h"

#include<algorithm>

QList<ElapsedTimer::ElapsedTimerAttribute> ElapsedTimer::elapsedTimerAttributes=QList<ElapsedTimer::ElapsedTimerAttribute>();

ElapsedTimer::ElapsedTimer(const QString &itemName){
    attrIter=std::find_if(elapsedTimerAttributes.begin(),elapsedTimerAttributes.end(),
        [&itemName](const ElapsedTimerAttribute &attr)->bool{
            return attr.itemName==itemName;
        }
    );

    if(attrIter==elapsedTimerAttributes.end()){
        attrIter=elapsedTimerAttributes.insert(elapsedTimerAttributes.end(),ElapsedTimerAttribute(itemName));
    }
}

long long ElapsedTimer::restart(){
    if(attrIter->elapsedTimer.isValid()){
        attrIter->lastValue=attrIter->elapsedTimer.restart();
    }else{
        attrIter->elapsedTimer.start();
        attrIter->lastValue=0;
    }

    return attrIter->lastValue;
}

QList<QPair<QString,long long>> ElapsedTimer::getAll(){
    QList<QPair<QString,long long>> list;

    for(auto attr:elapsedTimerAttributes){
        list.append(QPair<QString,long long>(attr.itemName,attr.lastValue));
    }

    return list;
}

ElapsedTimer::ElapsedTimerAttribute_::ElapsedTimerAttribute_(const QString &itemName){
    this->itemName=itemName;
}
