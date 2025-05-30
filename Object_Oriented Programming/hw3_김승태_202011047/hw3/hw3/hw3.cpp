#include <iostream>
#include <string>

using namespace std;

class IArticleSub;

class IArticlePub {
private:
    string pub_name;
    int pub_id;
    string recent_contents;
    static int static_pub_counter;
protected:
    IArticleSub* sub_list[5];
    int numOfSub = 0;
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
};

class IArticleSub {
private:
    string sub_name;
    int sub_id;
    static int static_sub_counter;
    IArticlePub* recent_article_pub;
    string recent_article_contents;
protected:
    IArticlePub* pub_list[5];
    int numOfPub = 0;
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
        for (int i = 0; i < numOfPub; i++) {
            pub_list[i]->NotifyDetachResponse(this); 
        }
    };

    void Attach(IArticlePub* publisher) {
        int find = -1;
        for (int i = 0; i < numOfPub; i++) {
            if (pub_list[i] == publisher) {
                find = i;
            }
        }
        if (find != -1) {
            cout << "[ERROR]: already subscribe" << publisher->getPubName() << endl;
            return;
        }
        cout << "[Attach] Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ") is attached to Sub (" << this->getSubName() << "," << this->getSubID() << ")" << endl;
        pub_list[numOfPub] = publisher;
        numOfPub++;
        publisher->NotifyAttachResponse(this);
    }

    void AttachResponse(IArticlePub* publisher) {
        int find = -1;
        for (int i = 0; i < numOfPub; i++) {
            if (pub_list[i] == publisher) {
                find = i;
            }
        }
        if (find != -1) {
            cout << "[ERROR]: already subscribe" << publisher->getPubName() << endl;
            return;
        }
        cout << "[Attach] Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ") is attached to Sub (" << this->getSubName() << "," << this->getSubID() << ")" << endl;
        pub_list[numOfPub] = publisher;
        numOfPub++;
    }

    void Detach(IArticlePub* p_pub) {
        int find = -1;
        for (int i = 0; i < numOfPub; i++) {
            if (pub_list[i] == p_pub) {
                find = i;
            }
        }
        if (find == -1) {
            cout << "[ERROR]: There is no publisher named" << p_pub->getPubName() << endl;
        }
        else {
            cout << "[Sub] (" << this->getSubName() << "," << this->getSubID() << ") unsubscribes [Pub] (" << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << endl;
            for (int j = find; j < numOfPub - 1; j++) {
                pub_list[j] = pub_list[j + 1];
            }
            pub_list[numOfPub - 1] = nullptr;
            numOfPub--;
            p_pub->NotifyDetachResponse(this);
        }
    }

    void DetachResponse(IArticlePub* p_pub) {
        int find = -1;
        for (int i = 0; i < numOfPub; i++) {
            if (pub_list[i] == p_pub) {
                find = i;
            }
        }
        if (find == -1) {
            cout << "[ERROR]: There is no publisher named" << p_pub->getPubName() << endl;
        }
        else {
            cout << "[Sub] (" << this->getSubName() << "," << this->getSubID() << ") unsubscribes [Pub] (" << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << endl;
            for (int j = find; j < numOfPub - 1; j++) {
                pub_list[j] = pub_list[j + 1];
            }
            pub_list[numOfPub - 1] = nullptr;
            numOfPub--;
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

    int getPubSize() {
        return this->numOfPub;
    }
    void PrintAllPub() {
        for (int i = 0; i < numOfPub; i++) {
            cout << "[" << pub_list[i]->getPubName() << "," << pub_list[i]->getPubID() << "]";
        }
        cout << endl;
    }
};

int IArticleSub::static_sub_counter = 0;

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
    cout << "IArticlePub Destructor called" << endl;
    for (int i = 0; i < numOfSub; i++) {
        sub_list[i]->DetachResponse(this);
    }
}

void IArticlePub::NotifyAttachResponse(IArticleSub* subscriber) {
    int find = -1;
    for (int i = 0; i < numOfSub; i++) {
        if (sub_list[i] == subscriber) {
            find = i;
        }
    }
    if (find != -1) {
        cout << "[ERROR]: already exists" << endl;
    }
    else {
        cout << "[Attach] Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ") is attached to Pub (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
        sub_list[numOfSub++] = subscriber;
    }
}

void IArticlePub::NotifyAttach(IArticleSub* subscriber) {
    int find = -1;
    for (int i = 0; i < numOfSub; i++) {
        if (sub_list[i] == subscriber) {
            find = i;
        }
    }
    if (find != -1) {
        cout << "[ERROR]: already exists" << endl;
    }
    else {
        cout << "[Attach] Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ") is attached to Pub (" << this->getPubName() << "," << this->getPubID() << ")" << endl;
        sub_list[numOfSub++] = subscriber;
        subscriber->AttachResponse(this);
    }
}

void IArticlePub::NotifyDetach(IArticleSub* subscriber) {
    int find = -1;
    for (int i = 0; i < numOfSub; i++) {
        if (sub_list[i] == subscriber) {
            find = i;
        }
    }
    if (find == -1) {
        cout << "[ERROR]: There is no subscriber named" << subscriber->getSubName() << endl;
    }
    else {
        cout << "[Pub] (" << this->getPubName() << "," << this->getPubID() << ") detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
        for (int j = find; j < numOfSub - 1; j++) {
            sub_list[j] = sub_list[j + 1];
        }
        sub_list[numOfSub - 1] = nullptr;
        numOfSub--;
        subscriber->DetachResponse(this);
    }
}
void IArticlePub::NotifyDetachResponse(IArticleSub* subscriber) {
    int find = -1;
    for (int i = 0; i < numOfSub; i++) {
        if (sub_list[i] == subscriber) {
            find = i;
        }
    }
    if (find == -1) {
        cout << "[ERROR]: There is no subscriber named" << subscriber->getSubName() << endl;
    }
    else {
        cout << "[Pub] (" << this->getPubName() << "," << this->getPubID() << ") detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
        for (int j = find; j < numOfSub - 1; j++) {
            sub_list[j] = sub_list[j + 1];
        }
        sub_list[numOfSub - 1] = nullptr;
        numOfSub--;
    }
}
void IArticlePub::Notify() {
    for (int i = 0; i < numOfSub; i++) {
        sub_list[i]->Update(this, recent_contents);
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
    return this->numOfSub;
}
void IArticlePub::PrintAllSub() {
    for (int i = 0; i < numOfSub; i++) {
        cout << "[" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << "]";
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
    void Event() override{
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
    IArticlePub* cnnPub_upcasting = cnnPub;
    //Jenny subscribe DGIST, BBC
    IArticleSub* jennySub = new IArticleSub("Jenny", dgistPub);
    bbcPub->NotifyAttach(jennySub);
    //Tom subscribe BBC, CNN
    IArticleSub* tomSub = new IArticleSub("Tom");
    bbcPub->NotifyAttach(tomSub);
    cnnPub_upcasting->NotifyAttach(tomSub);
    //Kate subscribe DGIST, BBC, CNN
    IArticleSub* kateSub = new IArticleSub("Kate", dgistPub);
    bbcPub->NotifyAttach(kateSub);
    cnnPub_upcasting->NotifyAttach(kateSub);
    cout << "All Sub of (" << dgistPub->getPubName() << "," << dgistPub->getPubID() << "): ";
    dgistPub->PrintAllSub();

    cout << "All Sub of (" << bbcPub->getPubName() << "," << bbcPub->getPubID() << "): ";
    bbcPub->PrintAllSub();

    cout << "All Sub of (" << cnnPub_upcasting->getPubName() << "," << cnnPub_upcasting -> getPubID() << "): ";
    cnnPub_upcasting->PrintAllSub();

    bbcPub->Event();
    cnnPub_upcasting->Event();
    dgistPub->Event();
    dgistPub->CheerUp();
    kateSub->Detach(bbcPub);
    cout << "All Pub of (" << jennySub->getSubName() << "," << jennySub->getSubID() << "): ";
    jennySub->PrintAllPub();

    cout << "All Pub of (" << tomSub->getSubName() << "," << tomSub->getSubID() << "): ";
    tomSub->PrintAllPub();

    cout << "All Pub of (" << kateSub->getSubName() << "," << kateSub->getSubID() << "): ";
    kateSub->PrintAllPub();

    cout << "=========DGIST Notify ===========" << endl;
    dgistPub->updatePubContents("Welcome New DGIST students");

    cout << "=========BBC Notify ===========" << endl;
    bbcPub->updatePubContents("Mr. Son scored at Tottenham");

    cout << "=========CNN Notify ===========" << endl;
    cnnPub_upcasting->updatePubContents("New York city celebrates Christmas");
    cout << "=========DELETING [tomSub]===========" << endl;
    delete tomSub;
    cout << "=========DGIST Notify ===========" << endl;
    dgistPub->updatePubContents("Welcome New DGIST students");

    cout << "=========BBC Notify ===========" << endl;
    bbcPub->updatePubContents("Mr. Son scored at Tottenham");

    cout << "=========CNN Notify ===========" << endl;
    cnnPub_upcasting->updatePubContents("New York city celebrates Christmas");

    cout << "=========Delete all others ===========" << endl;
    delete dgistPub;
    delete bbcPub;
    delete cnnPub;
    delete jennySub;

    delete kateSub;

    return 0;
}
