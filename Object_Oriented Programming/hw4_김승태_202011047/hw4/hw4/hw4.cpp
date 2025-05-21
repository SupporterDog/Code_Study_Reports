#include <iostream>
#include <string>
#include <vector>

using namespace std;

class IArticleSub;

class IArticlePub{
private:
    string pub_name;
    int pub_id;
    string recent_contents;
    static int static_pub_counter;
protected:
    vector <IArticleSub*> sub_list;
public:
    IArticlePub(const string name);
    IArticlePub(const string name, const string con);
    ~IArticlePub();
    void NotifyAttachResponse(IArticleSub* subscriber);
    void NotifyAttach(IArticleSub* subscriber);
    void NotifyDetach(IArticleSub* subscriber);
    void NotifyDetachResponse(IArticleSub* subscriber);
    void Notify();
    void updatePubContents(string c);
    int getPubID();
    string getPubName();
    int getSubSize(); 
    void PrintAllSub();  
    virtual void Event() = 0;
    friend IArticlePub& operator<<(IArticlePub& publisher, const string& contents);
};

class IArticleSub {
private:
    string sub_name;
    int sub_id;
    static int static_sub_counter;
    IArticlePub* recent_article_pub;
    string recent_article_contents;
protected:
    vector <IArticlePub*> pub_list;
public:

    IArticleSub(string name) {
        this->sub_name = name;
        ++IArticleSub::static_sub_counter;
        this->sub_id = IArticleSub::static_sub_counter;
        cout << "[Constructor]New Sub Created: (" << this->getSubName() << "," << this->getSubID() << ")" << endl;
    }

    IArticleSub(string name, IArticlePub* articlePub) {
        this->sub_name = name;
        ++IArticleSub::static_sub_counter;
        this->sub_id = IArticleSub::static_sub_counter;
        cout << "[Constructor]New Sub Created: (" << this->getSubName() << "," << this->getSubID() << ")" << endl;
        Attach(articlePub);
    }

    ~IArticleSub() {
        cout << "IArticleSub Destructor called" << endl;
        for (auto* pub : pub_list) {
            pub->NotifyDetachResponse(this);
        }
    };

    void Attach(IArticlePub* publisher) {
        auto it = find(pub_list.begin(), pub_list.end(), publisher);
        if (it != pub_list.end()) {
            cout << "[ERROR]: already subscribes " << publisher->getPubName() << endl;
            return;
        }

        cout << "[Attach] Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ") is attached to Sub ("
            << this->getSubName() << "," << this->getSubID() << ")" << endl;

        pub_list.push_back(publisher);
        publisher->NotifyAttachResponse(this);
    }

    void AttachResponse(IArticlePub* publisher) {
        auto it = std::find(pub_list.begin(), pub_list.end(), publisher);
        if (it != pub_list.end()) {
            std::cout << "[ERROR]: already subscribes " << publisher->getPubName() << std::endl;
            return;
        }

        std::cout << "[Attach] Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ") is attached to Sub ("
            << this->getSubName() << "," << this->getSubID() << ")" << std::endl;

        pub_list.push_back(publisher);
    }

    void Detach(IArticlePub* p_pub) {
        auto it = std::find(pub_list.begin(), pub_list.end(), p_pub);
        if (it == pub_list.end()) {
            std::cout << "[ERROR]: There is no publisher named " << p_pub->getPubName() << std::endl;
        }
        else {
            std::cout << "[Sub] (" << this->getSubName() << "," << this->getSubID() << ") unsubscribes [Pub] ("
                << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << std::endl;

            pub_list.erase(it);
            p_pub->NotifyDetachResponse(this);
        }
    }

    void DetachResponse(IArticlePub* p_pub) {
        auto it = std::find(pub_list.begin(), pub_list.end(), p_pub);
        if (it == pub_list.end()) {
            std::cout << "[ERROR]: There is no publisher named " << p_pub->getPubName() << std::endl;
        }
        else {
            std::cout << "[Sub] (" << this->getSubName() << "," << this->getSubID() << ") unsubscribes [Pub] ("
                << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << std::endl;

            pub_list.erase(it);
        }
    }
    void Update(IArticlePub* publisher, const string contents) {
        this->recent_article_pub = publisher;
        this->recent_article_contents = contents;
        PrintContents();
    }

    void PrintContents() {
        cout << "Sub (" << this->sub_name << "," << this->sub_id << ")'s latest subscribed news is \"" << this->recent_article_contents << "\" by " << this->recent_article_pub->getPubName() << endl;
    }

    string getSubName() {
        return this->sub_name;
    }

    int getSubID() {
        return this->sub_id;
    }

    int getPubSize() const {
        return static_cast<int>(pub_list.size());
    }

    void PrintAllPub() const {
        for (IArticlePub* pub : pub_list) {
            cout << "[" << pub->getPubName() << "," << pub->getPubID() << "]";
        }
        cout << endl;
    }
    friend IArticleSub& operator>>(IArticleSub& subscriber, IArticlePub* publisher);
};

int IArticleSub::static_sub_counter = 0;

IArticlePub& operator<<(IArticlePub& publisher, const string& contents) {
    publisher.updatePubContents(contents);
    return publisher;
}

IArticleSub& operator>>(IArticleSub& subscriber, IArticlePub* publisher) {
    subscriber.Attach(publisher);
    return subscriber;
}


IArticlePub::IArticlePub(const string name) {
    this->pub_name = name;
    this->pub_id = ++IArticlePub::static_pub_counter;
    cout << "[Constructor]New Pub Created: (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
}

IArticlePub::IArticlePub(const string name, const string con) {
    this->pub_name = name;
    this->recent_contents = con;
    this->pub_id = ++IArticlePub::static_pub_counter;
    cout << "[Constructor]New Pub Created: (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
}

IArticlePub::~IArticlePub() {
    std::cout << "IArticlePub Destructor called" << std::endl;
    for (auto* sub : sub_list) {
        sub->DetachResponse(this);
    }
}

void IArticlePub::NotifyAttachResponse(IArticleSub* subscriber) {
    auto it = find(sub_list.begin(), sub_list.end(), subscriber);
    if (it != sub_list.end()) {
        cout << "[ERROR]: already exists" << endl;
    }
    else {
        cout << "[Attach] Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ") is attached to Pub (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
        sub_list.push_back(subscriber);
    }
}

void IArticlePub::NotifyAttach(IArticleSub* subscriber) {
    auto it = find(sub_list.begin(), sub_list.end(), subscriber);
    if (it != sub_list.end()) {
        cout << "[ERROR]: already exists" << endl;
    }
    else {
        cout << "[Attach] Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ") is attached to Pub (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
        sub_list.push_back(subscriber);
        subscriber->AttachResponse(this);
    }
}

void IArticlePub::NotifyDetach(IArticleSub* subscriber) {
    auto it = find(sub_list.begin(), sub_list.end(), subscriber);
    if (it == sub_list.end()) {
        cout << "[ERROR]: There is no subscriber named " << subscriber->getSubName() << endl;
    }
    else {
        cout << "[Pub] (" << this->getPubName() << "," << this->getPubID() << ") detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
        sub_list.erase(it);
        subscriber->DetachResponse(this);
    }
}

void IArticlePub::NotifyDetachResponse(IArticleSub* subscriber) {
    auto it = find(sub_list.begin(), sub_list.end(), subscriber);
    if (it == sub_list.end()) {
        cout << "[ERROR]: There is no subscriber named " << subscriber->getSubName() << endl;
    }
    else {
        cout << "[Pub] (" << this->getPubName() << "," << this->getPubID() << ") detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
        sub_list.erase(it);
    }
}

void IArticlePub::Notify() {
    for (auto* sub : sub_list) {
        sub->Update(this, recent_contents);
    }
}
void IArticlePub::updatePubContents(string c) {
    this->recent_contents = c;
    Notify();
}
int IArticlePub::getPubID() {
    return this->pub_id;
}
string IArticlePub::getPubName() {
    return this->pub_name;
}
int IArticlePub::getSubSize() {
    return static_cast<int>(sub_list.size());
}

void IArticlePub::PrintAllSub() {
    for (IArticleSub* sub : sub_list) {
        cout << "[" << sub->getSubName() << "," << sub->getSubID() << "]";
    }
    cout << endl;
}


int IArticlePub::static_pub_counter = 0;

class dgist_press : public IArticlePub {
public:
    dgist_press() : IArticlePub("DGIST") {
        cout << "dgist press constructor1 is called" << endl;
    }
    dgist_press(const string con) : IArticlePub("DGIST", con) {
        cout << "dgist press constructor2 is called" << endl;
    }
    void Event() override {
        cout << "The Event is totally swindle" << endl;
    }
    void CheerUp() {
        for (int i = 0; i < getSubSize(); i++) {
            IArticleSub* subscriber = sub_list[i];
            subscriber->Update(this, "I Love you guys");
        }
    }
    ~dgist_press() {
        cout << "DGIST PRESS is destructed" << endl;
    }
};

class BBC : public IArticlePub {
public:
    BBC() : IArticlePub("BBC") {
        cout << "BBC Constructor1 is called" << endl;
    }
    BBC(const string con) : IArticlePub("BBC", con) {
        cout << "BBC Constructor2 is called" << endl;
    }
    void Advertisement() {
        cout << "BBC NEWS is coming soon!!" << endl;
    }
    void Event() override {
        if (getSubSize() > 0) {
            // Select the first subscriber
            IArticleSub* winner = sub_list[0];
            cout << "The Event winner is (" << winner->getSubName() << "," << winner->getSubID() << ")" << endl;
        }
        else {
            cout << "No subscribers for the event" << endl;
        }
    }
    ~BBC() {
        cout << "BBC is destructed" << endl;
    }
};

class CNN : public IArticlePub {
public:
    CNN() :IArticlePub("CNN") {
        cout << "CNN Constructor1 is called" << endl;
    }
    CNN(const string con) : IArticlePub("CNN", con) {
        cout << "CNN Constructor2 is called" << endl;
    }
    void Event() override {
        if (getSubSize() > 0) {
            IArticleSub* winner = sub_list[getSubSize() - 1];
            cout << "The Event winner is (" << winner->getSubName() << winner->getSubID() << ")" << endl;
        }
        else {
            cout << "No subscribers for the event" << endl;
        }
    }
    ~CNN() {
        cout << "CNN is destructed" << endl;
    }
};






int main() {
    dgist_press* dgistPub = new dgist_press(string("this is dgist pub"));
    BBC* bbcPub = new BBC();
    CNN* cnnPub = new CNN();
    //Jenny subscribe DGIST, BBC
    IArticleSub* jennySub = new IArticleSub("Jenny", dgistPub);
    *jennySub >> bbcPub; //Operator Overloading
    //Tom subscribe BBC, CNN
    IArticleSub* tomSub = new IArticleSub("Tom");
    *tomSub >> bbcPub; //Operator Overloading
    *tomSub >> cnnPub; //Operator Overloading
    //Kate subscribe DGIST, BBC, CNN
    IArticleSub* kateSub = new IArticleSub("Kate", dgistPub);
    *kateSub >> bbcPub; //Operator Overloading
    *kateSub >> bbcPub; //Operator Overloading
    *kateSub >> cnnPub; //Operator Overloading
    cout << "All Sub of (" << dgistPub->getPubName() << "," << dgistPub->getPubID() << "):";
        dgistPub->PrintAllSub();

    cout << "All Sub of (" << bbcPub->getPubName() << "," << bbcPub->getPubID() << "): ";
    bbcPub->PrintAllSub();

    cout << "All Sub of (" << cnnPub->getPubName() << "," << cnnPub->getPubID() << "): ";
    cnnPub->PrintAllSub();

    bbcPub->Event();
    dgistPub->CheerUp();
    kateSub->Detach(bbcPub);
    cout << "All Pub of (" << jennySub->getSubName() << "," << jennySub->getSubID() << "):";
        jennySub->PrintAllPub();

    cout << "All Pub of (" << tomSub->getSubName() << "," << tomSub->getSubID() << "): ";
    tomSub->PrintAllPub();

    cout << "All Pub of (" << kateSub->getSubName() << "," << kateSub->getSubID() << "): ";
    kateSub->PrintAllPub();

    cout << "=========DGIST Notify ===========" << endl;
    *dgistPub << "Welcome New DGIST students"; //Operator Overloading

    cout << "=========BBC Notify ===========" << endl;
    *bbcPub << "Mr. Son scored at Tottenham"; //Operator Overloading

    cout << "=========CNN Notify ===========" << endl;
    *cnnPub << "New York city celebrates Christmas"; //Operator Overloading
    cout << "=========DELETING [tomSub]===========" << endl;
    delete tomSub;
    cout << "=========DGIST Notify ===========" << endl;
    *dgistPub << "Welcome New DGIST students"; //Operator Overloading

    cout << "=========BBC Notify ===========" << endl;
    *bbcPub << "Mr. Son scored at Tottenham"; //Operator Overloading

    cout << "=========CNN Notify ===========" << endl;
    *cnnPub << "New York city celebrates Christmas"; //Operator Overloading

    cout << "=========Delete all others ===========" << endl;
    delete dgistPub;
    delete bbcPub;
    delete cnnPub;
    delete jennySub;
    //delete tomSub;
    delete kateSub;

    return 0;
}

