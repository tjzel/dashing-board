#ifndef DATA_PROVIDER_HPP
#define DATA_PROVIDER_HPP

class DataProvider {
public:
  int get();
  DataProvider(int initialValue, int step, int sign, int bottomThreshold, int topThreshold);

private:
  int _value;
  int _step;
  int _sign;
  int _bottomThreshold;
  int _topThreshold;
};

#endif // DATA_PROVIDER_HPP
