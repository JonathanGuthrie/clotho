#if !defined(_DATASOURCE_HPP_INCLUDED_)
#define _DATASOURCE_HPP_INCLUDED_

class DataSource {
private:

public:
  DataSource(void);
  virtual ~DataSource();

  // This is passed a buffer of size length and returns the number of
  // characters that were placed in the buffer.  If fetch ever returns
  // zero, that is interpreted as if the sender is out of data.  If that
  // is not the case, the client has to make arrangements for calling
  // wantsToSend again.
  virtual size_t fetch(uint8_t *buffer, size_t &length);
};

#endif // _DATASOURCE_HPP_INCLUDED_
