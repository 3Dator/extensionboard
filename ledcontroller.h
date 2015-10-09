typedef struct{
	uint16_t ms;
	void (*updateFunction)(uint16_t);
	uint16_t counter;
};
