#ifndef CRC_H
#define CRC_H

#include <cstdint>
#include <string>

#define CHUNK_SIZE 4098

class CRC
{
public:
	CRC();
	uint32_t calc_crc(std::string& file_data);
private:
	int nchar_;
	int crc_;
	void update(const unsigned char* buf, uint32_t size);
	uint32_t digest() const;
};


#endif // !CRC_H
