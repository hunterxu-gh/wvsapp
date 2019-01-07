#include "TSingleton.h"
template <class T>
T* TSingleton<T>::GetInstance() {
	return this->ms_pInstance;
}
template <class T>
TSingleton<T>::TSingleton(unsigned int *addy) {
    this->ms_pInstance = reinterpret_cast<T*>(*addy);
}
