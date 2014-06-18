/*
 *  Worker.cpp - Is the owner of a Processor thread
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of liveMediaStreamer.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors:  David Cassany <david.cassany@i2cat.net>,
 *            
 */

#ifndef _WORKER_HH
#define _WORKER_HH

#include <atomic>
#include <thread>
#include <list>
#include "Frame.hh"

#define MAX_SLAVE 16

class Runnable;
class LiveMediaRunnable;

class Worker {
    
public:
    Worker(Runnable *processor_, unsigned int maxFps = 0);
    Worker();
    void setProcessor(Runnable *processor, unsigned int maxFps = 0);

    
    bool start();
    bool isRunning();
    virtual void stop();
    virtual void enable();
    virtual void disable();
    bool isEnabled();
    void setFps(int maxFps);
    
protected:
    virtual void process();
    Runnable *processor;
    std::thread thread;
    std::atomic<bool> run;
    std::atomic<bool> enabled;
    //TODO: owuld be good to make it atomic, but not sure if it is lock-free
    unsigned int frameTime; //microseconds
};

class LiveMediaWorker : public Worker {
public:
    LiveMediaWorker(Runnable *processor_);
    void enable() {};
    void disable() {};
    void stop();
private:
    void process();
};

class Slave : public Worker {
public:
	Slave(int id_, Runnable *processor_, unsigned int maxFps = 0);
	int getId(){return id;};
	bool getFinished(){return finished;};
	void setFalse();
	void setFrame(Frame* org);
protected:
	void process();
private:
	int id;
	std::atomic<bool> finished;
	Frame* origin;
	
};

class Master : public Worker {
public:
	Master(Runnable *processor_, unsigned int maxFps = 0);
	bool addSlave(Slave *slave_);
	void removeSlave(int id);
protected:
	void process();
private:
	std::list<Slave*> slaves;
	bool allFinished();
	void processAll();
};

class Runnable {
    
public:
    virtual bool processFrame(Frame *org = NULL, bool removeFrame = false) = 0;
	virtual void removeFrames() = 0;
	virtual bool hasFrames() = 0;
	virtual Frame* getFrame() = 0;
    virtual void stop() = 0;
};

#endif
