#ifndef TEST_H_
#define TEST_H_

typedef struct
{
#define CONST_INPUT_PARAMS_CHECK_ID 0xff
    uint8_t     check_id;
    uint32_t    chunk_size;
    uint32_t    chunk_count;

} __attribute__((packed)) test_params_t;

#endif /* TEST_H_ */
