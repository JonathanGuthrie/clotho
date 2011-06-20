#include <string>

#include "datasource.hpp"

DataSource::DataSource(uint8_t *buffer, size_t length) : m_startOffset(0), m_bufferLen(length) {
  m_buffer = new uint8_t[m_bufferLen];
  memcpy(m_buffer, buffer, m_bufferLen);
}


DataSource::~DataSource() {
  delete[] m_buffer;
}


DataSource::Fetch(size_t &length) {
  
}
