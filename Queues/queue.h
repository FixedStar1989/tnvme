/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "tnvme.h"
#include "trackable.h"
#include "../Singletons/memBuffer.h"
#include "../Utils/fileSystem.h"
#include "../Exception/frmwkEx.h"


/**
* This class is the base class to all other queue classes. It is not meant to
* be instantiated. This class contains all things common to queues at a high
* level.  After instantiation by a child the Init() method must be called be
* to attain something useful.
*
* @note This class may throw exceptions.
*/
class Queue : public Trackable
{
public:
    /**
     * @param fd Pass the opened file descriptor for the device under test
     * @param objBeingCreated Pass the type of object this child class is
     */
    Queue(int fd, Trackable::ObjType objBeingCreated);
    virtual ~Queue();

    /// A Q can either be a CQ or an SQ
    virtual bool GetIsCQ() = 0;

    /// A Q can either be of administration or IO type
    bool GetIsAdmin() { return (mQId == 0); }

    /// A Q can either be contiguous or discontiguous
    bool GetIsContig();

    uint16_t GetQId() { return mQId; }
    uint32_t GetNumEntries() { return mNumEntries; }
    uint16_t GetEntrySize() { return mEntrySize; }

    /// Get the memory which encompasses the Q's contents
    uint8_t const *GetQBuffer();
    /// Get the total size in bytes of the Q's memory contents
    uint32_t GetQSize() { return (mEntrySize * mNumEntries); }

    /**
     * Send the entire contents of this Q to the logging endpoint
     * @param bufOffset Pass the offset byte for which to start dumping
     * @param length Pass the number of bytes to dump, ULONG_MAX implies all
     */
    void Log(uint32_t bufOffset = 0, unsigned long length = ULONG_MAX);

    /**
     * Send the entire contents of this Q to the named file.
     * @param filename Pass the filename as generated by macro
     *      FileSystem::PrepLogFile().
     * @param fileHdr Pass a custom file header description to dump
     */
    virtual void Dump(DumpFilename filename, string fileHdr);


protected:
    /// file descriptor to the device under test
    int mFd;

    /// mQId==0 implies admin Q
    uint16_t mQId;
    uint16_t mEntrySize;
    uint32_t mNumEntries;

    /// dnvme alloc'd Q memory which is mmap'd back into user space as RO
    uint8_t *mContigBuf;
    /// tnvme alloc'd Q memory, handed off to this obj during init'ing
    SharedMemBufferPtr mDiscontigBuf;

    /**
     * Initialize this object
     * @param qId Pass the queue's ID
     * @param entrySize Pass the number of bytes encompassing each element
     * @param numEntries Pass the number of elements within the Q
     */
    void Init(uint16_t qId, uint16_t entrySize, uint32_t numEntries);


private:
    Queue();
};


#endif
