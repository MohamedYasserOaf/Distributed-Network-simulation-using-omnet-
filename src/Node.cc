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

#include "Node.h"



#include <vector>
#include <bitset>
#include <cmath>
using namespace std;

Define_Module(Node);

bitset<8> parity_bits_original(0);
int size_payload_original;


//////////////////////////////////simulates error
void Node::sendmessage(MyMessage *msg,int dest){


    int lossrand = uniform(0,1)*100;
               EV<<"  frame loss random is "<<std::to_string(lossrand)<<endl;
               if(lossrand < par("frameloss").intValue()){

               }
               else {
                                int bitrand=uniform(0,1)*100;
                                EV<<"  bit random is "<<std::to_string(bitrand)<<endl;
                                if(bitrand < par("biterror").intValue())
                                {
                                  std::string mypayload= msg->getM_Payload();
                                  int rand2=uniform(0,mypayload.size()-1);
                                  EV<<"   rand2 is "<<std::to_string(rand2)<< " payload of rand2 is -> " << mypayload[rand2] <<endl;
                                  if (mypayload[rand2]=='0')
                                  mypayload[rand2]='1';
                                  else if(mypayload[rand2]=='1')
                                  mypayload[rand2]='0';
                                  msg->setM_Payload(mypayload.c_str());
                                }
                                EV<<"  payload is "<<msg->getM_Payload()<<endl;
    int delayrand=uniform(0,1)*100;
               int duprand=uniform(0,1)*100;
               EV<<"  delay random is "<<std::to_string(delayrand)<<endl;
               EV<<"  dupilaction random is "<<std::to_string(duprand)<<endl;

               if(duprand < par("duplicate").intValue()){
                   if(delayrand < par("delay").intValue()){
                       sendDelayed(msg, par("delay").doubleValue(),"outs", dest);
                       MyMessage* msgsend=msg->dup();
                       sendDelayed(msgsend, par("delay").doubleValue(), "outs", dest);
                   }else{
                       sendDelayed(msg, 1,"outs", dest);
                       MyMessage* msgsend=msg->dup();
                       sendDelayed(msgsend, 1, "outs", dest);
                   }

               }
               else{
                   if(delayrand < par("delay").intValue()){
                       sendDelayed(msg, par("delay").doubleValue(),"outs", dest);
                   }
                   else{
                       sendDelayed(msg, 1,"outs", dest);
                   }
               }

               }
    }




////////////////////////////////applies hamming code
string Node::hamming(string payload){

    cout << "payload size is  " << payload.size() << "\n";
    int r=0;
    int parity;
    while(((payload.size())+1+r) >= pow(2,r)){     ////finding the redundant bits
        r++;
    }
    cout << " r is " << r << "\n";

    int Pbits_position[r];                       //// identifying the p bits location
            for(int i=0 ; i<r;i++){

                Pbits_position[i]=pow(2,i);
            }

int msg_plus_parity[payload.size()+r];
int j=0;
int k=0;
for(int i=1;i<=payload.size()+r;i++){           ///compose a message of data + parity (parity still not calculated)
if(i==Pbits_position[j]){
    msg_plus_parity[i]=-1;
    j++;

}else {
    msg_plus_parity[i]=(int)payload[k]-48;
    k++;
}

}

k = 0;
int x, min, max = 0;
//finding parity bit
for (int i = 1; i <= payload.size() + r; i = pow (2, k)){
k++;
parity = 0;
j = i;
x = i;
min = 1;
max = i;
while ( j <= payload.size() + r){
  for (x = j; max >= min && x <= payload.size() + r; min++, x++){
      if (msg_plus_parity[x] == 1)
          parity = parity + 1;;
  }
  j = x + i;
  min = 1;
}

//checking for even parity
if (parity % 2 == 0){
  msg_plus_parity[i] = 0;
}
else{
  msg_plus_parity[i] = 1;
}
}

std::stringstream temp;
for(int i=1;i<=payload.size()+r;i++){
temp << msg_plus_parity[i];
}
return temp.str();

}

//////////////////////////////////
bool Node::get_availability(){

return(par("isAvailable").boolValue());

}

void Node::set_availability(bool availability){

    par("isAvailable").setBoolValue(availability);

}

void Node::initialize()
{
    int rand11 = uniform(0,1)*100;
    EV<<"  frame loss random is "<<std::to_string(rand11)<<endl;




    if(rand11 < 99){

        string M;
        w=par("windowsize").intValue();
        sl=w-1;
        if(getIndex()==0){

            par("isAvailable").setBoolValue(true);
            scheduleAt(simTime() + 30.0, new cMessage("pair"));


        int a=getParentModule()->par("n").intValue();
        int rand_node= uniform(0,1)*(a-1);
        int rand_file=uniform(1,5);

//////////////////////////////////////////////
        MyMessage * msg;
                string line;
                int rand_file1=uniform(1,7);
                cout << "../node"+to_string(rand_file1)+".txt" <<endl;
                file.open("../node"+to_string(rand_file1)+".txt", ios::in);
                if (file.is_open())
                    while (getline(file, line)) {

                            std::deque<std::bitset<8>> vec;

                            for (std::bitset<8> foo : line)
                                vec.push_back(foo);

                            std::stringstream temp;


                            for (int i=0;i<vec.size();i++){
                                temp << vec[i];
                            }
                            M=temp.str();

                        data.push_back(M);

                    }
                for(int i=0;i<w;i++){
                   msg = new MyMessage();
                   msg->setM_Payload(hamming(data[i]).c_str());
                   msg->setM_Type(1);//0 data - 1 ACK - 2 NACK
                   msg->setSeq_Num(i%(w+2));
                   msg->setCharacter_count((hamming(data[i]).size())+1);
                   msg->setNode_Number(getIndex());

                   window.push_back(msg);
                }
                cMessage* msg1=new cMessage("timer");
                scheduleAt (simTime()+0.001,msg1);
                file.close();
///////////////////////////////////////////////


         //string message_w_hamming= hamming(M);
        /////////////////////////////////////////////////// initialize msg
        //msg = new MyMessage("");
      /*  msg->setM_Payload(message_w_hamming.c_str());
        /////////////
        string rec_payload=msg->getM_Payload();
        size_payload_original=rec_payload.size();
        /////////////
        msg->setNode_Number(getIndex());
        msg->setM_Type(1);
        msg->setSeq_Num(0);
        msg->setMycheckbits(parity_bits_original);
        msg->setCharacter_count(size_payload_original+1); // character count*/
        /////////////////////////////////////////////////////
        cModule *mod = getParentModule()->getSubmodule("nodes" , rand_node);
        Node *pointer_to_node=check_and_cast<Node*>(mod);
        bool other_node_available= pointer_to_node->get_availability();

        if(other_node_available==false){
        pointer_to_node->set_availability(true);


        if(rand_node==getIndex()){
            return;

        }

        if(rand_node>=getIndex()){

            f=rand_node-1;
        }
        else f=rand_node;
        }
        }

}


        }





void Node::handleMessage(cMessage *msg)
{

    if(msg->isSelfMessage()){

        if(strcmp(msg->getName(),"timer")==0){
                    int sent=s%(w+2) ;
                if(!window.empty()){
                if(s<=sl){
                    if(s<sf) s=sf;
                    sendmessage(window[(s-sf)]->dup(),f);
                  s++;}
                delete msg;
               msg=new cMessage("timer");
                  scheduleAt (simTime()+1,msg);
                  msg=new cMessage(to_string(sent).c_str());
                  scheduleAt (simTime()+2.0001,msg);
                }}
                else if(strcmp(msg->getName(),"pair")!=0) {
                    int re=atoi(msg->getName());
                       EV << " time out for frame number at node "<< getIndex()  <<endl;
                    if(sf<=sl){
                    if((((sf%(w+2)<sl%(w+2)) && (re<=sl%(w+2) && re>=sf%(w+2)) )||((sf%(w+2)>sl%(w+2))&& !(re>sl%(w+2) && re<sf%(w+2)) )|| ((sf%(w+2)==sl%(w+2))&& re==sl%(w+2) ))&&s>re){
                       EV<<"current S "<<s<<"  ";
                        s=re;

                    EV<<"WENT BACK to "<<s<<endl;
                    }}
                    delete msg;
                }
                else{
///////////////////////////////////////
                    delete msg;

/////////////////////////////////////////////////////

        int rand11 = uniform(0,1)*100;
        EV<<"  frame loss random is "<<std::to_string(rand11)<<endl;
        if(rand11 < 50){
            scheduleAt(simTime() + 30.0, new cMessage("pair"));
            return;
        }
            int a=getParentModule()->par("n").intValue();
            int rand_node= uniform(0,1)*(a-1);

            cModule *mod = getParentModule()->getSubmodule("nodes" , rand_node);
            Node *pointer_to_node=check_and_cast<Node*>(mod);
            bool other_node_available= pointer_to_node->get_availability();
            int n=getParentModule()->par("n").intValue();
            int count=0;
            while (other_node_available || rand_node==getIndex()){
                if(count>n){
                    scheduleAt(simTime() + 1, new cMessage("pair"));
                    return;
                }
                rand_node++;
                rand_node=rand_node % a;
                mod = getParentModule()->getSubmodule("nodes" , rand_node);
                pointer_to_node=check_and_cast<Node*>(mod);
                other_node_available= pointer_to_node->get_availability();
                count++;
            }
            pointer_to_node->set_availability(true);

            EV << "rand node is" << rand_node <<endl;

            MyMessage * msg_type_0 = new MyMessage("");
            string message_w_hamming= hamming("00000000");
            msg_type_0->setM_Payload(message_w_hamming.c_str());

            /////////////
            /////////////
            msg_type_0->setNode_Number(getIndex());
            msg_type_0->setM_Type(0);
            msg_type_0->setSeq_Num(0);
            msg_type_0->setMycheckbits(parity_bits_original);
            msg_type_0->setCharacter_count(message_w_hamming.size()+1); // character count

            if(rand_node>=getIndex()){

                sendDelayed(msg_type_0,1,"outs",rand_node-1);
            }
            else sendDelayed(msg_type_0,1,"outs",rand_node);

        return;
        }}
////////////////////////////////////////////////////////////////////
    if(!msg->isSelfMessage()){

        MyMessage *mmsg = check_and_cast<MyMessage *>(msg);



///////////////////////////////////////////////////////
        if(mmsg->getM_Type()==0){
            string M;
            int node_to_send=mmsg->getNode_Number();
            scheduleAt(simTime() + 30.0, new cMessage("pair"));
            ///////////////////////////////////////////////////
            //////////////////////////////////////////////
                    MyMessage * msg;
                            string line;
                            int rand_file=uniform(1,7);
                            s=0;sf=0;sl=w-1;
                            cout << "../node"+to_string(rand_file)+".txt" <<endl;
                            file.open("../node"+to_string(rand_file)+".txt", ios::in);
                            if (file.is_open())
                                while (getline(file, line)) {

                                        std::deque<std::bitset<8>> vec;

                                        for (std::bitset<8> foo : line)
                                            vec.push_back(foo);

                                        std::stringstream temp;


                                        for (int i=0;i<vec.size();i++){
                                            temp << vec[i];
                                        }
                                        M=temp.str();

                                    data.push_back(M);

                                }
                            for(int i=0;i<w;i++){
                               msg = new MyMessage();
                               msg->setM_Payload(hamming(data[i]).c_str());
                               msg->setM_Type(1);//0 data - 1 ACK - 2 NACK
                               msg->setSeq_Num(i%(w+2));
                               msg->setCharacter_count((hamming(data[i]).size())+1);
                               msg->setNode_Number(getIndex());

                               window.push_back(msg);
                            }
                            cMessage* msg1=new cMessage("timer");
                            scheduleAt (simTime()+0.001,msg1);
                            file.close();
            ///////////////////////////////////////////////
           /* MyMessage * msg_send = new MyMessage("");
            string message_w_hamming= hamming("01010101");
            msg_send->setM_Payload(message_w_hamming.c_str());
            /////////////
            string rec_payload=msg_send->getM_Payload();
            int size_payload=rec_payload.size();
            msg_send->setM_Type(1);
            msg_send->setSeq_Num(0);
            msg_send->setMycheckbits(0);
            msg_send->setNode_Number(getIndex());
            msg_send->setCharacter_count(size_payload+1);*/
            /////////////////////////////////////////////////////




                            int a=getParentModule()->par("n").intValue();
                        int rand_node= uniform(0,1)*(a-1);
                        int n=getParentModule()->par("n").intValue();
                                    int count=0;
                        cModule *mod = getParentModule()->getSubmodule("nodes" , rand_node);
                        Node *pointer_to_node=check_and_cast<Node*>(mod);
                        bool other_node_available= pointer_to_node->get_availability();

                        while (other_node_available || rand_node==getIndex()){
                                        if(count>n){
                                            scheduleAt(simTime() + 1, new cMessage("pair"));
                                            return;
                                        }
                                        rand_node++;
                                        rand_node=rand_node % a;
                                        mod = getParentModule()->getSubmodule("nodes" , rand_node);
                                        pointer_to_node=check_and_cast<Node*>(mod);
                                        other_node_available= pointer_to_node->get_availability();
                                        count++;
                                    }
                        pointer_to_node->set_availability(true);

                 /*       if(rand_node==getIndex()){
                            return;

                        }*/
                        if(rand_node>getIndex()){

                            f=rand_node-1;
                        }else f=rand_node;
                        delete mmsg;
            }else if (mmsg->getM_Type()==1){


                ///////////////////////////////////////////////
                int node_to_send=mmsg->getNode_Number();
                int sq=mmsg->getSeq_Num();

                if(r%(w+2)==sq){

                           string payload=mmsg->getM_Payload();

                           //////////////////////////////////////////////////////////////////////hamming detection -- may remove
                           int k = 0;
                           int j = 0;
                           int z=0;
                           int error_pos=0;
                           int x, min, max = 0;
                           bitset<1> parity(0);

                           //finding parity bit
                           for (int i = 1; i <= payload.size(); i = pow (2, k)){
                           k++;
                           parity=0;
                           j = i;
                           x = i;
                           min = 1;
                           max = i;
                           while ( j <= payload.size()){
                             for (x = j; max >= min && x <= payload.size(); min++, x++){
                                 parity^=payload[x-1];
                                 cout << payload[x-1] << "";

                             }
                             j = x + i;
                             min = 1;
                               }
                           error_pos+=(int)parity.to_ulong()*pow(2,z);
                           EV << "parity check is---> " << (int)parity.to_ulong()  << " multiplied by  " <<  pow(2,z) << " equals " << error_pos<<endl;
                           z++;
                           }
                           if(error_pos==0){EV<< " No parity error detected " <<endl;}
                           if(payload[error_pos-1]=='0'){
                               payload[error_pos-1]='1';
                           }else if(payload[error_pos-1]=='1'){
                               payload[error_pos-1]='0';
                           }

                           EV << "corrected message is  " << payload << endl;
                           //stringstream temp_msg;
                           string msg_received="";
                           int m=0;

                           for(int i=1;i<=payload.size();i++){
                               if(i==pow(2,m)){
                                   m++;
                               }
                               else{

                                   msg_received=msg_received+payload[i-1];
                               }
                           }

                          EV << "MSG RECEIVED IS " <<  msg_received <<endl;




                               std::stringstream sstream(msg_received);
                               std::string output;
                               while(sstream.good())
                               {
                                   std::bitset<8> bits;
                                   sstream >> bits;
                                   char c = char(bits.to_ulong());
                                   output += c;
                               }

                           EV<<"received message with sequence number ...   ";
                           EV << mmsg->getSeq_Num();
                           EV << " message type is " <<"";
                           EV << mmsg->getM_Type() <<"";
                           EV<<"  and payload after correction  ";
                           EV<< "at node " <<  getIndex() << " |||" << "  from node   " << mmsg->getNode_Number() <<endl;
                           EV<< " The output is --> " << output <<""<<endl;


                MyMessage * msg_send = new MyMessage("");

                string message_w_hamming= hamming("01010101");
                msg_send->setM_Payload(message_w_hamming.c_str());
                /////////////
                string rec_payload=msg_send->getM_Payload();
                msg_send->setM_Type(2);
                msg_send->setSeq_Num(sq);
                msg_send->setMycheckbits(0);
                msg_send->setNode_Number(getIndex());
                msg_send->setCharacter_count(message_w_hamming.size()+1);
                /////////////////////////////////////////////////////


                r++;

                if(node_to_send==getIndex()){
                    return;

                }

                if(node_to_send>getIndex()){
                    sendmessage(msg_send,node_to_send-1);
                }else sendmessage(msg_send,node_to_send);
                }
                delete mmsg;

            }
            else if (mmsg->getM_Type()==2){

                int sq=mmsg->getSeq_Num();
                MyMessage * msg;
                if(((sf%(w+2)<sl%(w+2)) && (sq<=sl%(w+2) && sq>=sf%(w+2)) )||((sf%(w+2)>sl%(w+2))&& !(sq>sl%(w+2) && sq<sf%(w+2)) ) || ((sf%(w+2)==sl%(w+2))&& sq==sl%(w+2) )) {


                                   EV<<"ACK"<<sq<<endl;
                                   EV<<"received message with sequence number ...   ";
                                   EV << sq;
                                   EV<<"and type ";
                                   EV<<mmsg->getM_Type();
                                   delete mmsg;

                    int check;
                    for(int i=0 ;i<w;i++){

                            check=window.front()->getSeq_Num();
                            delete window.front();

                            window.pop_front();
                            sf++;
                            if(data.size()>(sl+1)){
                            sl++;




                            msg = new MyMessage();
                            msg->setM_Payload(hamming(data[sl]).c_str());
                            msg->setM_Type(1);//0 data - 1 ACK - 2 NACK
                            msg->setSeq_Num(sl%(w+2));
                            msg->setCharacter_count((hamming(data[i]).size())+1);
                            msg->setNode_Number(getIndex());
                            window.push_back(msg);
                            }
                            if(check==sq)
                            break;

                    }
                    if(window.empty()){
                        data.clear();
                        MyMessage * end=new MyMessage("");
                        end->setM_Type(10);
                        end->setNode_Number(getIndex());
                        sendDelayed(end,4,"outs",f);
                    }
                }


            }
            else if (mmsg->getM_Type()==10){

                cModule *mod = getParentModule()->getSubmodule("nodes",mmsg->getNode_Number());
                Node *pointer_to_node=check_and_cast<Node*>(mod);
               pointer_to_node->set_availability(false);
                r=0;
                set_availability(false);
                delete mmsg;
            }

        }



    }







