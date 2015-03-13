#ifndef HADTU_H
#define HADTU_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum HadTu_Decoding {
	HadTu_queueDecoding = 3 << 16 | 99
};

static uint32_t *HadTu_hdr(const void *my);
static size_t HadTu_hdrLen(void);
static size_t HadTu_hdrSize(void);
static int HadTu_isSwapped(const void *my);
static uint32_t HadTu_hdrValue(const void *my, unsigned idx);
static void HadTu_setHdrValue(const void *my, unsigned idx, uint32_t value);
static uint32_t HadTu_decoding(const void *my);
static void HadTu_setDecoding(void *my, uint32_t decoding);
static unsigned HadTu_alignment(const void *my);
static uint32_t HadTu_size(const void *my);
static void HadTu_setSize(void *my, uint32_t size);
static size_t HadTu_paddedSize(const void *my);
static void *HadTu_begin(const void *my);
static void *HadTu_end(const void *my);
static size_t HadTu_dataSize(const void *my);
static void *HadTu_data(const void *my);
static void *HadTu_next(const void *my, const void *hadTu);
char *HadTu_2charP(const void *my);

enum HadTuIdx {
	HadTuIdx_size,
	HadTuIdx_decoding,
	HadTuIdx_data
};

static uint32_t *HadTu_hdr(const void *my)
{
	return (uint32_t *) my;
}

static size_t HadTu_hdrLen(void)
{
	return HadTuIdx_data;
}

static size_t HadTu_hdrSize(void)
{
	return HadTu_hdrLen() * sizeof(uint32_t);
}

static int HadTu_isSwapped(const void *my)
{
	uint32_t *hdr = HadTu_hdr(my);

	return hdr[HadTuIdx_decoding] > 0xffffff;
}

static uint32_t HadTu_hdrValue(const void *my, unsigned idx)
{
	uint32_t *hdr = HadTu_hdr(my);

	if (HadTu_isSwapped(my)) {
		uint32_t tmp0;
		uint32_t tmp1;

		tmp0 = hdr[idx];
		((char *) &tmp1)[0] = ((char *) &tmp0)[3];
		((char *) &tmp1)[1] = ((char *) &tmp0)[2];
		((char *) &tmp1)[2] = ((char *) &tmp0)[1];
		((char *) &tmp1)[3] = ((char *) &tmp0)[0];
		return tmp1;
	} else {
		return hdr[idx];
	}
}

static void HadTu_setHdrValue(const void *my, unsigned idx, uint32_t value)
{
	uint32_t *hdr = HadTu_hdr(my);

	hdr[idx] = value;
}

static uint32_t HadTu_decoding(const void *my)
{
	return HadTu_hdrValue(my, HadTuIdx_decoding);
}

static void HadTu_setDecoding(void *my, uint32_t decoding)
{
	HadTu_setHdrValue(my, HadTuIdx_decoding, decoding);
}

static unsigned HadTu_alignment(const void *my)
{
	return 1 << (HadTu_decoding(my) >> 16 & 0xff);
}

static uint32_t HadTu_size(const void *my)
{
	return HadTu_hdrValue(my, HadTuIdx_size);
}

static void HadTu_setSize(void *my, uint32_t size)
{
	HadTu_setHdrValue(my, HadTuIdx_size, size);
}

static size_t HadTu_paddedSize(const void *my)
{
	uint32_t size;
	uint32_t alignment;

	size = HadTu_size(my);
	alignment = HadTu_alignment(my);
	return (size & (alignment - 1))
		? (size & ~(alignment - 1)) + alignment : size;
}

static void *HadTu_begin(const void *my)
{
	return (void *) my;
}

static void *HadTu_end(const void *my)
{
	return (void *) ((char *) my + HadTu_paddedSize(my));
}

static size_t HadTu_dataSize(const void *my)
{
	return HadTu_size(my) - HadTu_hdrSize();
}

static void *HadTu_data(const void *my)
{
	return (void *) ((char *) my + HadTu_hdrSize());
}

static void *HadTu_next(const void *my, const void *hadTu)
{
	uint32_t size;
	uint32_t alignment;
	size_t paddedSize;

	size = HadTu_size(hadTu);
	alignment = HadTu_alignment(my);
	paddedSize = (size & (alignment - 1))
		? (size & ~(alignment - 1)) + alignment : size;

	return (void *) ((char *) hadTu + paddedSize);
}

#ifdef __cplusplus
}
#endif
#endif
