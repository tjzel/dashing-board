#ifndef DATA_PROVIDER_HPP
#define DATA_PROVIDER_HPP

class DataProvider {
public:
  int get();
  DataProvider(int initialValue, int step, int sign, int bottomThreshold, int topThreshold);

private:
  int value_;
  int step_;
  int sign_;
  int bottomThreshold_;
  int topThreshold_;
};

#endif // DATA_PROVIDER_HPP
