#ifndef CONFIG_H
#define CONFIG_H

#include "EEPROM.h"

template <class T>
class Config : protected EEPROMClass {
public:
  Config();
  ~Config();
  void save();
  T* operator->();
private:
  T* _dataPtr;
};

template <class T>
Config<T>::Config() {
  begin(sizeof(T));
  _dataPtr = (T*)getDataPtr();
}

template <class T>
Config<T>::~Config() {
  end();
}

template <class T>
T* Config<T>::operator->() {
  return _dataPtr;
}

template <class T>
void Config<T>::save() {
  getDataPtr();
  commit();
}

#endif
