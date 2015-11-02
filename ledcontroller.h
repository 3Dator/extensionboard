void setColor(uint8_t r, uint8_t g, uint8_t b);
void setColorAnalog(uint8_t r, uint8_t g, uint8_t b);

typedef struct{
	uint16_t ms;
	void (*updateFunction)(uint16_t);
	uint16_t counter;
};


