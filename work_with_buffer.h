#ifndef _WORK_WITH_BUFFER_H_
#define _WORK_WITH_BUFFER_H_


int BufferToText(struct LineParams** text, char* buffer, const size_t buf_size);
void FreeBuffer(char** buffer);

#endif // _WORK_WITH_BUFFER_H_
