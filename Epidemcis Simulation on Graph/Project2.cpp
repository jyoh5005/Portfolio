// 20192912 오준엽
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <deque>

#define MAX_VIXS 100
#define INF 9999
#define a 5

static int m = 1;
static std::mt19937 gen = std::mt19937((unsigned int)time(0));
static std::uniform_real_distribution<> newEdge(0, 1);
static std::uniform_int_distribution<> alphaEdge(0, a);
static std::uniform_int_distribution<> alphaRelationShip(0, 2);
static std::uniform_real_distribution<> danger(0, 1);
static std::uniform_real_distribution<> infect(1.4, 2.5); // 감염력 추정치
static std::uniform_real_distribution<> meeting(0, m);

// enums
enum State{
    CLEAN,                  // 청결
    ASYMPTOMATIC_INFECT,    // 무증상 감염
    ASYMPTOMATIC_CARRIER,   // 무증상 전파
    PATIENT,                // 환자
    RECOVERED,              // 회복
    DEAD,                   // 사망
    IMMUNE                  // 면역
};
enum RelationShip{
    FAMILY,                 // 가족
    FRIEND,                 // 친구
    COWORKER,               // 동료
    CARE,                   // 치료
    UNKNOWN                 // 미정
};
// class
class Node{
    private:
        int id;
        int state;
        std::deque<Node*> edge;
        std::deque<int> relationship;
        float hygiene; // 위생도
        float dangerous; // 중증도
        bool isolation;
        int job;
        void changeState(int infectVal);
    public:
        Node(int ID);
        ~Node();
        int getId();
        int getState();
        std::deque<Node*> getEdge();
        std::deque<int> getRelationShip();
        float getHygiene();
        bool getIsolation();
        int getJob();
        // setid
        void setEdge(Node* n);
        void setRelationShip(int val);
        void setState(int val);
        void setHygiene(int HG);
        void setIsolation(bool b);
        void setJob(int val);
        bool isEdge(Node* n);
        int whereEdge(Node* n);
        void INFECT();
};
void Node::changeState(int infectVal){
    if(state == DEAD || state == IMMUNE ){ return; }
    if(state == CLEAN || state == RECOVERED && hygiene < infectVal){ state = ASYMPTOMATIC_INFECT;  return;}
    dangerous = danger(gen);
    if(state == ASYMPTOMATIC_INFECT){
        if(dangerous > 0.3){ state = ASYMPTOMATIC_CARRIER; }
        if(dangerous > 0.1){ state = PATIENT; }
        if(dangerous < 0.05){ state = RECOVERED; hygiene += 0.1; }
    }
    if(state == ASYMPTOMATIC_CARRIER){
        if(dangerous > 0.5){ state = PATIENT; }
        if(dangerous < 0.05){ state = RECOVERED; }
    }
    if(state == PATIENT){
        if(dangerous > 0.99){ state = DEAD; }
        if(dangerous < 0.05){ state = RECOVERED; hygiene += 0.2; }
    }
    if(hygiene > 2.5){ state = IMMUNE; }
}
Node::Node(int ID) : id(ID), state(CLEAN), hygiene(1), isolation(false){}
Node::~Node(){}
int Node::getId(){ return id; }
int Node::getState(){ return state; }
std::deque<Node*> Node::getEdge(){ return edge; }
std::deque<int> Node::getRelationShip(){ return relationship; }
float Node::getHygiene(){ return hygiene; }
bool Node::getIsolation(){ return isolation; }
int Node::getJob(){ return job; }
void Node::setEdge(Node* n){ edge.push_back(n); }
void Node::setRelationShip(int val){ relationship.push_back(val); }
void Node::setState(int val){ state = val; }
void Node::setHygiene(int HG){ hygiene = HG; }
void Node::setIsolation(bool b){ isolation = b; }
void Node::setJob(int val){ job = val; }
bool Node::isEdge(Node* n){
    for(int i = 0; i < edge.size(); i++){
        if(edge[i] == n){
            return true;
        }
    }
    return false;
}
int Node::whereEdge(Node* n){
    for(int i = 0; i < edge.size(); i++){
        if(edge[i] == n){
            return i;
        }
    }
    return -1;
}
void Node::INFECT(){
    if(state == DEAD || state == IMMUNE ){ return; }
    int infecVal = 0;
    for(int i = 0; i < edge.size(); i++){
        if(edge[i] -> state == ASYMPTOMATIC_CARRIER || (edge[i] -> state == PATIENT && !isolation)){
            if(relationship[i] == FAMILY){
                infecVal += infect(gen);
            }
            if(relationship[i] == FRIEND && meeting(gen) > 0.2){
                infecVal += infect(gen);
            }
        }
    }
    changeState(infecVal);
}
// class
class BAM_Graph{
    protected:
        std::deque<Node*> Vertex;
        int company;
        int entertainment;
        int hospital;
        int fm;
        int fd;

    public:
        BAM_Graph();
        ~BAM_Graph();
        void Create_BAM_Graph();
        void setCompany(int val);
        void setEntertainment(int val);
        void setHospital(int val);
        void save(std::string filename);
};
BAM_Graph::BAM_Graph() : company(0), entertainment(0), hospital(0){
    for(int i = 0; i < MAX_VIXS; i++){
        Vertex.push_back(new Node(i));
    }
}
BAM_Graph::~BAM_Graph(){
    for(int i = 0; i < MAX_VIXS; i++){
        delete Vertex[i];
    }
}
void BAM_Graph::Create_BAM_Graph(){
    // 최소 엣지 갯수 설정
    while(true){
        std::cout << "최소 가족 관계수(2 이상): ";
        std::cin >> fm;
        if(fm >= 2){
            break;
        }
        std::cout << "가족을 2 이상의 수로 입력해 주세요\n";
    }
    while(true){
        std::cout << "최소 친구 관계수(2 이상): ";
        std::cin >> fd;
        if(fd >= 2){
            break;
        }
        std::cout << "친구를 2 이상의 수로 입력해 주세요\n";
    }
    // 기본 노드 fm + fd 개 생성 
    for(int i = 0; i < fm+fd+a; i++){
        // 관계
        int checkR[3] = {0};
        std::deque<int> newR;
        for(int j = i+1; j < fm+fd;){
            newR.push_front(alphaRelationShip(gen));
            if(checkR[0] > fm && checkR[1] > fd){ j++; continue; }
            switch(newR.front()){
            case FAMILY:
                if(checkR[newR.front()]++ > fm){
                    newR.push_back(newR.front());
                    newR.pop_front();
                }
                else{ j++; }
                break;
            case FRIEND:
                if(checkR[newR.front()]++ > fd){
                    newR.push_back(newR.front());
                    newR.pop_front();
                }
                else{ j++; }
                break;
            }
        }
        for(int j = i+1; j < fm+fd; j++){
            Vertex[i] -> setEdge(Vertex[j]);
            Vertex[j] -> setEdge(Vertex[i]);
            Vertex[i] -> setRelationShip(newR[j]);
            Vertex[j] -> setRelationShip(newR[j]);
        }
    }
    // 모든 엣지 갯수 저장
    int totalEdge = (fm+fd) * (fm+fd+a);
    // 새 노드 추가
    for(int i = fm+fd+a; i < MAX_VIXS; i++){
        int alpha;
        alpha = alphaEdge(gen);
        // 랜덤 관계 설정 배열
        int checkR[3] = {0};
        std::deque<int> newR;
        for(int j = 0; j < fm+fd+alpha;){
            newR.push_front(alphaRelationShip(gen));
            if(checkR[0] > fm && checkR[1] > fd){ j++; continue; }
            switch(newR.front()){
            case FAMILY:
                if(checkR[newR.front()]++ > fm){
                    newR.push_back(newR.front());
                    newR.pop_front();
                }
                else{ j++; }
                break;
            case FRIEND:
                if(checkR[newR.front()]++ > fd){
                    newR.push_back(newR.front());
                    newR.pop_front();
                }
                else{ j++; }
                break;
            }
        }
        // 랜덤 엣지
        std::uniform_int_distribution<> newEdge2Node(0, i-1); // idx 랜덤 선택 설정
        int idx = newEdge2Node(gen); // idx 랜덤 선택
        double p = newEdge(gen); // 확률 랜덤 선택
        for(int j = 0; j < fm+fd+alpha;){
            if(!Vertex[i] -> isEdge(Vertex[idx])){
                p -= (double) Vertex[idx] -> getEdge().size() / (double)totalEdge;
                if(p < 0){
                    Vertex[i] -> setEdge(Vertex[idx]);
                    Vertex[idx] -> setEdge(Vertex[i]);
                    Vertex[i] -> setRelationShip(newR[j]);
                    Vertex[idx] -> setRelationShip(newR[j]);
                    j++;
                }
            }
            if(++idx == i){ idx = 0; }
        }
        totalEdge += Vertex[i] -> getEdge().size(); // 새 노드의 엣지 갯수 추가
    }
}
void BAM_Graph::setCompany(int val){ company = val; }
void BAM_Graph::setEntertainment(int val){ entertainment = val; }
void BAM_Graph::setHospital(int val){ hospital = val; }
void BAM_Graph::save(std::string filename){
    std::ofstream ofs(filename);
    ofs << "최소 가족 관계: " << fm << ", 최소 친구 관계: " << fd << '\n';
    for(int i = 0; i < MAX_VIXS; i++){
        for(int j = 0; j < MAX_VIXS; j++){
            if(!Vertex[i] -> isEdge(Vertex[j])){
                ofs << "0_______ ";
                continue;
            }
            int k = Vertex[i] -> whereEdge(Vertex[j]);
            switch(Vertex[i] -> getRelationShip()[k]){
            case FAMILY:
                ofs << "FAMILY__ ";
                break;
            case FRIEND:
                ofs << "FRIEND__ ";
                break;
            case COWORKER:
                ofs << "COWORKER ";
                break;
            case CARE:
                ofs << "CARE____ ";
                break;
            case UNKNOWN:
                ofs << "UNKNOWN_ ";
                break;
            default:
                ofs << "0_______ ";
                break;
            }
        }
        ofs << '\n';
    }
    ofs.close();
}


class Pandemic : BAM_Graph{
    private:
        int day;
        std::deque<int> dead;
        std::deque<int> infected;
        int total_infected;
        std::deque<Node*> pre_vertex;
        int act[3] = {0};
    public:
        void Create_BAM_Graph();
        void active_Mask();
        void active_Isolation();
        void active_Social_Distancing();
        void Pandemic_START();
        void Pandemic_END();
        void save_Pandemic(std::string filename);
        void save(std::string filename);
        Pandemic();
        ~Pandemic();
};
void Pandemic::Create_BAM_Graph(){ BAM_Graph::Create_BAM_Graph(); }
void Pandemic::active_Mask(){
    for(int i = 0; i < MAX_VIXS; i++){
        Vertex[i] -> setHygiene(Vertex[i] -> getHygiene() + 0.5);
    }
}
void Pandemic::active_Isolation(){
    for(int i = 0; i < MAX_VIXS; i++){
        Vertex[i] -> setIsolation(true);
    }
}
void Pandemic::active_Social_Distancing(){ m = 0.5; } // static int m
void Pandemic::Pandemic_START(){
    day = 0;
    infected.push_back(0);
    dead.push_back(0);
    for(int i = 0; i < 10; i++){
        Vertex[i*10] -> setState(ASYMPTOMATIC_CARRIER);
    }
    int newInfected;
    int newDead;
    while(true){
        newInfected = 0;
        newDead = 0;
        for(int i = 0; i < MAX_VIXS; i++){
            Vertex[i] -> INFECT();
            if(Vertex[i] -> getState() == ASYMPTOMATIC_CARRIER || Vertex[i] -> getState() == PATIENT){
                newInfected++;
            }
            if(Vertex[i] -> getState() == DEAD){
                newDead++;
            }
        }
        day++;
        infected.push_back(newInfected);
        dead.push_back(newDead);

        if(day % 7 == 0){
            int active = 0;
            std::cout << "경과일: " << day << ", 확진자 수: " << newInfected << ", 사망자 수: " << newDead << '\n';
            std::cout << "조치를 취하시겠습니까? (0: 아무 조치를 취하지 않음, 1: 마스크 착용, 2: 격리 실시, 3: 사회적 거리두기 캠퍼인, 4: 시뮬레이션 종료) ";
            std::cin >> active;
            if(active == 1){ std::cout << "이제부터 사람들이 마스크를 착용합니다\n"; active_Mask(); act[0]= day; }
            if(active == 2){ std::cout << "이제부터 사람들이 격리됩니다\n"; active_Isolation(); act[1] = day; }
            if(active == 3){ std::cout << "이제부터 사람들이 사회적 거리두기를 실시합니다\n"; active_Social_Distancing(); act[2] = day;}
            if(active == 4){ std::cout << "시뮬레이션을 종료합니다\n"; Pandemic_END(); return; }
        }
    }
}
void Pandemic::Pandemic_END(){
    std::cout << "시뮬레이션 결과....\n";
    std::cout << "마스크 착용 시행일: " << act[0] << ", 격리 실시일: " << act[1] << ", 사회적 거리두기 캠페인 시행일: " << act[2] << '\n';
    for(int i = 0; i < day; i++){
        std::cout << "경과일: " << i << ", 확진자 수: " << infected[i] << ", 사망자 수: " << dead[i] << '\n';
    }
}
void Pandemic::save_Pandemic(std::string filename){
    std::ofstream ofs(filename);
    ofs << "마스크 착용 시행일: " << act[0] << ", 격리 실시일: " << act[1] << ", 사회적 거리두기 캠페인 시행일: " << act[2] << '\n';
    for(int i = 0; i < day; i++){    
        ofs << "경과일: " << i << ", 확진자 수: " << infected[i] << ", 사망자 수: " << dead[i] << '\n';
    }
    ofs.close();
}
void Pandemic::save(std::string filename){ BAM_Graph::save(filename); }
Pandemic::Pandemic(){}
Pandemic::~Pandemic(){}

int main(){
    std::string name = "s";
    char c[5] = {'1', '2', '3', '4', '5'};
    for(int i = 0; i < 5; i++){
        Pandemic g;
        g.Create_BAM_Graph();
        g.Pandemic_START();
        g.save(name + c[i] + " graph.txt");
        g.save_Pandemic(name + c[i] + " result.txt");
    }
    return 0;
}