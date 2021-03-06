/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef GBA_THREAD_H
#define GBA_THREAD_H

#include "util/common.h"

#include "gba/gba.h"
#include "gba/input.h"
#include "gba/supervisor/overrides.h"

#include "util/threading.h"

struct GBAThread;
struct GBAArguments;
struct GBACheatSet;
struct GBAOptions;

typedef void (*ThreadCallback)(struct GBAThread* threadContext);

enum ThreadState {
	THREAD_INITIALIZED = -1,
	THREAD_RUNNING = 0,
	THREAD_INTERRUPTED,
	THREAD_INTERRUPTING,
	THREAD_PAUSED,
	THREAD_PAUSING,
	THREAD_RUN_ON,
	THREAD_RESETING,
	THREAD_EXITING,
	THREAD_SHUTDOWN,
	THREAD_CRASHED
};

struct GBASync {
	int videoFramePending;
	bool videoFrameWait;
	int videoFrameSkip;
	bool videoFrameOn;
	Mutex videoFrameMutex;
	Condition videoFrameAvailableCond;
	Condition videoFrameRequiredCond;

	bool audioWait;
	Condition audioRequiredCond;
	Mutex audioBufferMutex;
};

struct GBAThread {
	// Output
	enum ThreadState state;
	struct GBA* gba;
	struct ARMCore* cpu;

	// Input
	struct GBAVideoRenderer* renderer;
	struct GBASIODriverSet sioDrivers;
	struct ARMDebugger* debugger;
	struct VDir* gameDir;
	struct VDir* stateDir;
	struct VFile* rom;
	struct VFile* save;
	struct VFile* bios;
	struct VFile* patch;
	struct VFile* cheatsFile;
	const char* fname;
	const char* movie;
	int activeKeys;
	struct GBAAVStream* stream;
	struct Configuration* overrides;
	enum GBAIdleLoopOptimization idleOptimization;

	bool hasOverride;
	struct GBACartridgeOverride override;

	// Run-time options
	int frameskip;
	float fpsTarget;
	size_t audioBuffers;
	bool skipBios;
	int volume;
	bool mute;

	// Threading state
	Thread thread;

	Mutex stateMutex;
	Condition stateCond;
	enum ThreadState savedState;
	int interruptDepth;

	GBALogHandler logHandler;
	int logLevel;
	ThreadCallback startCallback;
	ThreadCallback cleanCallback;
	ThreadCallback frameCallback;
	void* userData;
	void (*run)(struct GBAThread*);

	struct GBASync sync;

	int rewindBufferSize;
	int rewindBufferCapacity;
	int rewindBufferInterval;
	int rewindBufferNext;
	struct GBASerializedState** rewindBuffer;
	int rewindBufferWriteOffset;
	uint8_t* rewindScreenBuffer;

	struct GBACheatDevice* cheats;
};

void GBAMapOptionsToContext(const struct GBAOptions*, struct GBAThread*);
void GBAMapArgumentsToContext(const struct GBAArguments*, struct GBAThread*);

bool GBAThreadStart(struct GBAThread* threadContext);
bool GBAThreadHasStarted(struct GBAThread* threadContext);
bool GBAThreadHasExited(struct GBAThread* threadContext);
bool GBAThreadHasCrashed(struct GBAThread* threadContext);
void GBAThreadEnd(struct GBAThread* threadContext);
void GBAThreadReset(struct GBAThread* threadContext);
void GBAThreadJoin(struct GBAThread* threadContext);

bool GBAThreadIsActive(struct GBAThread* threadContext);
void GBAThreadInterrupt(struct GBAThread* threadContext);
void GBAThreadContinue(struct GBAThread* threadContext);

void GBARunOnThread(struct GBAThread* threadContext, void (*run)(struct GBAThread*));

void GBAThreadPause(struct GBAThread* threadContext);
void GBAThreadUnpause(struct GBAThread* threadContext);
bool GBAThreadIsPaused(struct GBAThread* threadContext);
void GBAThreadTogglePause(struct GBAThread* threadContext);
void GBAThreadPauseFromThread(struct GBAThread* threadContext);
struct GBAThread* GBAThreadGetContext(void);

#ifdef USE_PNG
void GBAThreadTakeScreenshot(struct GBAThread* threadContext);
#endif

void GBASyncPostFrame(struct GBASync* sync);
void GBASyncForceFrame(struct GBASync* sync);
bool GBASyncWaitFrameStart(struct GBASync* sync, int frameskip);
void GBASyncWaitFrameEnd(struct GBASync* sync);
bool GBASyncDrawingFrame(struct GBASync* sync);

void GBASyncSuspendDrawing(struct GBASync* sync);
void GBASyncResumeDrawing(struct GBASync* sync);

void GBASyncProduceAudio(struct GBASync* sync, bool wait);
void GBASyncLockAudio(struct GBASync* sync);
void GBASyncUnlockAudio(struct GBASync* sync);
void GBASyncConsumeAudio(struct GBASync* sync);

#endif
