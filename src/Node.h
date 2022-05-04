//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __MESH_NODE_H_
#define __MESH_NODE_H_

#include <omnetpp.h>
#include <string>
#include <queue>
#include "MyMessage_m.h"
#include <fstream>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  protected:
    std::ifstream  file;
        std::vector<std::string> data;
        std::deque<MyMessage*>window;
        int s=0,r=0,sf=0,sl;int f;
        int w;


    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual bool get_availability();
    virtual void set_availability(bool availability);
    virtual std::string hamming(std::string payload);
    virtual void sendmessage(MyMessage *msg,int dest);

};

#endif
