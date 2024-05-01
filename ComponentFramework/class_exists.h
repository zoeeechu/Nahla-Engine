
template<typename T>
class class_exists {
  typedef char yes[1];
  typedef char no [2];

  template<typename C>
  static yes& test(typename C::type*);

  template<typename>
  static no& test(...);  

public:
  static const bool value = sizeof(test<T>(0)) == sizeof(yes);
};
