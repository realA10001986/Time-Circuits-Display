/*
 * AudioFileSourceLoop
 * Read SD/SPIFFS/LittleFS file to be used by AudioGenerator
 * Reads file in a loop (for looped playback)
 * 
 * Thomas Winischhofer (A10001986), 2023
 *
 * Based on AudioFileSourceSD by Earle F. Philhower, III
 *
 */

#include "tc_global.h"
#include "AudioFileSourceLoop.h"



AudioFileSourceLoop::~AudioFileSourceLoop()
{
    if(f) f.close();
}

uint32_t AudioFileSourceLoop::read(void *data, uint32_t len)
{
    uint32_t glen = 0, rlen = len, g;
    
    switch(ftype) {
    case 1:
        glen = f.read((uint8_t *)data, len);
        if(!doPlayLoop || glen == len) return glen;
        seek(startPos, SEEK_SET);
        glen += f.read(((uint8_t *)data) + glen, len - glen);
        break;
    case 2:
        while(ftype && (len > glen)) {
            if(csegLen >= rlen) {
                csegLen -= rlen;
                g = c_read((uint8_t *)data, rlen);
            } else {
                g = c_read((uint8_t *)data, csegLen);
                seekNext();
            }
            data = (void *)((uint8_t *)data + g);
            glen += g;
            rlen -= g;
        }
    }

    return glen;
}

bool AudioFileSourceLoop::seek(int32_t pos, int dir)
{
    if(!f) return false;
    if(dir == SEEK_SET)      return f.seek(pos);
    else if(dir == SEEK_CUR) return f.seek(f.position() + pos);
    else if(dir == SEEK_END) return f.seek(f.size() + pos);
    return false;
}

bool AudioFileSourceLoop::open_c(const char *filename, const int16_t *segs)
{
    uint32_t temp[3];
    int32_t *ftoc;
    int gsi = *segs;
    int si = 0;

    if(toc) { free(toc); toc = NULL; }

    segIdx = *segs * 2;
    
    if((toc = (int32_t *)malloc(segIdx * 4))) {
        if(open(filename)) {
            if(read((uint8_t *)&temp[0], 12) == 12) {
                if((ftoc = (int32_t *)malloc(temp[2]))) {
                    if(read((uint8_t *)ftoc, temp[2]) == temp[2]) {
                        while(gsi) {
                            toc[si++] = ftoc[segs[gsi]] - ftoc[segs[gsi] + 1];
                            toc[si++] = ~ftoc[segs[gsi--]];
                        }
                        free(ftoc);
                        ftype = 2;
                        if(seekNext()) return true;
                    }
                }
            }
            close();
        }
        if(toc) { free(toc); toc = NULL; }
    }
    return false;
}

bool AudioFileSourceLoop::seekNext()
{
    if(!segIdx || !toc) {
        csegLen = csegOLen = ftype = 0;
        return false;
    }

    f.seek(toc[--segIdx]);
    
    csegOLen = csegLen = toc[--segIdx];

    return true;
}

uint32_t AudioFileSourceLoop::c_read(uint8_t *buf, uint32_t len)
{
    return (*c)(buf, f.read(buf, len), maxSegs);
}

// SD -----------------------------------------------

AudioFileSourceSDLoop::AudioFileSourceSDLoop()
{
}

/*
AudioFileSourceSDLoop::AudioFileSourceSDLoop(const char *filename)
{
    open(filename);
}
*/

bool AudioFileSourceSDLoop::open(const char *filename)
{
    f = SD.open(filename, FILE_READ);
    ftype = 1;
    return f;
}

// FlashFS -------------------------------------------

AudioFileSourceFSLoop::AudioFileSourceFSLoop()
{
}

/*
AudioFileSourceFSLoop::AudioFileSourceFSLoop(const char *filename)
{
    open(filename);
}
*/

bool AudioFileSourceFSLoop::open(const char *filename)
{
    f = LittleFS.open(filename, FILE_READ);
    ftype = 1;
    return f;
}
