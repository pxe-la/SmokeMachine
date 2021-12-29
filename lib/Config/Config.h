#ifndef CONFIG_H
# define CONFIG_H

#define NO_GLOBAL_EEPROM

#include "EEPROM.h"

template <typename T>
class Config : protected EEPROMClass {
public:
  Config();
  ~Config();
  void save();
  T* operator->();
private:
  T* _dataPtr;
};

template <typename T>
Config<T>::Config() {
  begin(sizeof(T));
  _dataPtr = getDataPtr();
}

template <typename T>
Config<T>::~Config() {
  end();
}

template <typename T>
T* Config<T>::operator->() {
  return _dataPtr;
}

template <typename T>
void Config<T>::save() {
  getDataPtr();
  commit();
}

#endif
