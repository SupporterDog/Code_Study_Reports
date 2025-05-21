#include <iostream>
#include <string>
#pragma once
using namespace std;

class IArticlePub;
class IArticleSub;

class IArticlePub {
private:
	string pub_name;
	int pub_id;
	string recent_contents;
	IArticleSub* sub_list[5];
	int numOfSub = 0;
	static int static_pub_counter;
public:
	IArticlePub(const string name);
	IArticlePub(const string name, const string con);
	~IArticlePub();
	void NotifyAttach(IArticleSub* subscriber);
	void NotifyAttachResponse(IArticleSub* subscriber);
	void NotifyDetach(IArticleSub* subscriber);
	void NotifyDetachResponse(IArticleSub* subscriber);
	void updatePubContents(string c);
	void Notify();
	int getPubID();
	string getPubName();
	int getSubSize();
	void PrintAllSub();
};
int IArticlePub::static_pub_counter = 0;

class IArticleSub {
private:
	string sub_name;
	int sub_id;
	string recent_article_contents;
	IArticlePub* recent_article_pub;
	IArticlePub* pub_list[5];
	int numOfPub = 0;
	static int static_sub_counter;
public:
	IArticleSub(string name);
	IArticleSub(string name, IArticlePub* articlePub);
	~IArticleSub();
	void Attach(IArticlePub* publisher);
	void AttachResponse(IArticlePub* publisher);
	void Detach(IArticlePub* p_pub);
	void DetachResponse(IArticlePub* p_pub);
	void Update(IArticlePub* publisher, const string contents);
	void PrintContents();
	string getSubName();
	int getSubID();
	int getPubSize();
	void PrintAllPub();
};
int IArticleSub::static_sub_counter = 0;

IArticlePub::IArticlePub(const string name) {
	pub_name = name;
	static_pub_counter += 1;
	pub_id = static_pub_counter;
	for (int i = 0; i < 5; i++) {
		sub_list[i] = nullptr;
	}
	cout << "[Constructor]New Pub Created : (" << pub_name << "," << pub_id << ")" << endl;
}

IArticlePub::IArticlePub(const string name, const string con) {
	pub_name = name;
	static_pub_counter += 1;
	pub_id = static_pub_counter;
	for (int i = 0; i < 5; i++) {
		sub_list[i] = nullptr;
	}
	recent_contents = con;
	cout << "[Constructor]New Pub Created : (" << pub_name << "," << pub_id << ")" << endl;
}

IArticlePub::~IArticlePub() {
	cout << "IarticlePub Destructor called" << endl;
	for (int i = 0; i < numOfSub; i++) {
		if (sub_list[i] != nullptr) {
			sub_list[i]->DetachResponse(this);
		}
	}
}

void IArticlePub::NotifyAttach(IArticleSub* subscriber) {
	sub_list[numOfSub] = subscriber;
	numOfSub += 1;
	subscriber->AttachResponse(this);
	cout << "[Attach] Pub (" << pub_name << "," << pub_id << ") is attached to Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;

}

void IArticlePub::NotifyAttachResponse(IArticleSub* subscriber) {
	sub_list[numOfSub] = subscriber;
	numOfSub += 1;
	cout << "[Attach] Pub (" << pub_name << "," << pub_id << ") is attached to Sub (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
}

void IArticlePub::NotifyDetach(IArticleSub* subscriber) {
	bool isthereSub = false;
	for (int i = 0; i < numOfSub; i++) {
		if (sub_list[i] == subscriber) {
			cout << "[Pub] (" << pub_name << "," << pub_id << ") detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
			sub_list[i] = nullptr;
			isthereSub = true;
			for (int j = i + 1; j < numOfSub; j++) {
				if (sub_list[j] != nullptr) {
					sub_list[j - 1] = sub_list[j];
					sub_list[j] = sub_list[i];
				}
			}
			numOfSub -= 1;
			subscriber->DetachResponse(this);
			break;
		}
	}
	if (!isthereSub) {
		cout << "NotifyDetach error" << endl;
	}
}

void IArticlePub::NotifyDetachResponse(IArticleSub* subscriber) {
	bool isthereSub = false;
	for (int i = 0; i < numOfSub; i++) {
		if (sub_list[i] == subscriber) {
			cout << "[Pub] (" << pub_name << "," << pub_id << ") " << "detach [Sub] (" << subscriber->getSubName() << "," << subscriber->getSubID() << ")" << endl;
			sub_list[i] = nullptr;
			isthereSub = true;
			for (int j = i + 1; j < numOfSub; j++) {
				if (sub_list[j] != nullptr) {
					sub_list[j - 1] = sub_list[j];
					sub_list[j] = sub_list[i];
				}
			}
			numOfSub -= 1;
			break;
		}

	}
}

void IArticlePub::updatePubContents(string c) {
	recent_contents = c;
	Notify();
}

void IArticlePub::Notify() {
	for (int i = 0; i < numOfSub; i++) {
		if (sub_list[i] != nullptr) {
			cout << "Sub (" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << ")'s latest subscribed news is \"" << recent_contents << "\" by " << pub_name << endl;
			sub_list[i]->Update(this, recent_contents);
		}
	}
}

int IArticlePub::getPubID() {
	return pub_id;
}

string IArticlePub::getPubName() {
	return pub_name;
}

int IArticlePub::getSubSize() {
	return numOfSub;
}
void IArticlePub::PrintAllSub() {
	for (int i = 0; i < numOfSub; i++) {
		if (sub_list[i] != nullptr) {
			cout << "[" << sub_list[i]->getSubName() << "," << sub_list[i]->getSubID() << "]";
		}
	}
	cout << endl;
}

IArticleSub::IArticleSub(string name) {
	sub_name = name;
	static_sub_counter += 1;
	sub_id = static_sub_counter;
	for (int i = 0; i < 5; i++) {
		pub_list[i] = nullptr;
	}
	cout << "[Constructor]New Sub Created: (" << sub_name << "," << static_sub_counter << ")" << endl;
}
IArticleSub::IArticleSub(string name, IArticlePub* articlePub) {
	sub_name = name;
	static_sub_counter += 1;
	sub_id = static_sub_counter;
	for (int i = 0; i < 5; i++) {
		pub_list[i] = nullptr;
	}
	cout << "[Constructor]New Sub Created: (" << sub_name << "," << static_sub_counter << ")" << endl;
	Attach(articlePub);

}
IArticleSub::~IArticleSub() {
	cout << "IArticleSub Destructor called" << endl;
	int i = 0;
	while (pub_list[i] != nullptr) {
		pub_list[i]->NotifyDetach(this);
		i++;
	}
	/*for (int i = 0; i < numOfPub; i++) {
		if (pub_list[i] != nullptr) {
			cout << numOfPub;
			cout << i << endl;
			pub_list[i]->NotifyDetach(this);
			cout << "where" << endl;
		}
		else {
			cout << "out " << endl;
		}

	}*/
}
void IArticleSub::Attach(IArticlePub* publisher) {
	pub_list[numOfPub] = publisher;
	numOfPub += 1;

	publisher->NotifyAttachResponse(this);
	cout << "[Attach] Sub (" << sub_name << "," << sub_id << ") is attached to Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
}

void IArticleSub::AttachResponse(IArticlePub* publisher) {
	pub_list[numOfPub] = publisher;
	numOfPub += 1;
	cout << "[Attach] Sub (" << sub_name << "," << sub_id << ") is attached to Pub (" << publisher->getPubName() << "," << publisher->getPubID() << ")" << endl;
}
void IArticleSub::Detach(IArticlePub* p_pub) {
	bool isthereSub = false;
	for (int i = 0; i < numOfPub; i++) {
		if (pub_list[i] == p_pub) {
			cout << "[Sub] (" << sub_name << "," << sub_id << ") " << "unsubscribes [Pub] (" << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << endl;
			pub_list[i] = nullptr;
			isthereSub = true;
			for (int j = i + 1; j < numOfPub; j++) {
				if (pub_list[j] != nullptr) {
					pub_list[j - 1] = pub_list[j];
					pub_list[j] = pub_list[i];


				}
			}
			numOfPub -= 1;
			p_pub->NotifyDetachResponse(this);
			break;
		}
	}
	if (!isthereSub) {
		cout << "Detach error" << endl;
	}
}
void IArticleSub::DetachResponse(IArticlePub* p_pub) {
	bool isthereSub = false;
	for (int i = 0; i < numOfPub; i++) {
		if (pub_list[i] == p_pub) {
			cout << "[Sub] (" << sub_name << "," << sub_id << ") " << "unsubscribes [Pub] (" << p_pub->getPubName() << "," << p_pub->getPubID() << ")" << endl;
			pub_list[i] = nullptr;
			isthereSub = true;
			for (int j = i + 1; j < numOfPub; j++) {
				if (pub_list[j] != nullptr) {
					pub_list[j - 1] = pub_list[j];
					pub_list[j] = pub_list[i];
				}
			}
			numOfPub -= 1;
			break;
		}
	}
}
void IArticleSub::Update(IArticlePub* publisher, const string contents) {
	recent_article_pub = publisher;
	recent_article_contents = contents;
	//PrintContents();
}



void IArticleSub::PrintContents() {
	cout << "All Pub of (" << sub_name << "," << sub_id << "): ";
	for (int i = 0; i < numOfPub; i++) {
		if (pub_list[i] != nullptr) {
			cout << "[" << pub_list[i]->getPubName() << "," << pub_list[i]->getPubID() << "]" << endl;
		}
	}
}
string IArticleSub::getSubName() {
	return sub_name;
}
int IArticleSub::getSubID() {
	return sub_id;
}
int IArticleSub::getPubSize() {
	return numOfPub;
}
void IArticleSub::PrintAllPub() {
	for (int i = 0; i < numOfPub; i++) {
		if (pub_list[i] != nullptr) {
			cout << "[" << pub_list[i]->getPubName() << "," << pub_list[i]->getPubID() << "]";

		}
	}
	cout << endl;
}




int main() {
	IArticlePub* dgistPub = new IArticlePub("DGIST");
	IArticlePub* bbcPub = new IArticlePub("BBC");
	IArticlePub* cnnPub = new IArticlePub("CNN");
	cout << endl;

	//Jenny subscribe DGIST, BBC
	IArticleSub* jennySub = new IArticleSub("Jenny", dgistPub);
	bbcPub->NotifyAttach(jennySub);
	cout << endl;

	//Tom subscribe BBC, CNN
	IArticleSub* tomSub = new IArticleSub("Tom");
	bbcPub->NotifyAttach(tomSub);
	cnnPub->NotifyAttach(tomSub);
	cout << endl;

	//Kate subscribe DGIST, BBC, CNN
	IArticleSub* kateSub = new IArticleSub("Kate", dgistPub);
	bbcPub->NotifyAttach(kateSub);
	kateSub->Attach(cnnPub);
	cout << endl;

	cout << "All Sub of (" << dgistPub->getPubName() << "," << dgistPub->getPubID() << "): ";
	dgistPub->PrintAllSub();
	cout << "All Sub of (" << bbcPub->getPubName() << "," << bbcPub->getPubID() << "): ";
	bbcPub->PrintAllSub();
	cout << "All Sub of (" << cnnPub->getPubName() << "," << cnnPub->getPubID() << "): ";
	cnnPub->PrintAllSub();
	cout << endl;

	kateSub->Detach(bbcPub);
	cout << endl;

	cout << "All Pub of (" << jennySub->getSubName() << "," << jennySub->getSubID() << "): ";
	jennySub->PrintAllPub();
	cout << "All Pub of (" << tomSub->getSubName() << "," << tomSub->getSubID() << "): ";
	tomSub->PrintAllPub();
	cout << "All Pub of (" << kateSub->getSubName() << "," << kateSub->getSubID() << "): ";
	kateSub->PrintAllPub();
	cout << endl;

	cout << "=========DGIST Notify ===========" << endl;
	dgistPub->updatePubContents("Welcome New DGIST students");
	cout << endl;

	cout << "=========BBC Notify ===========" << endl;
	bbcPub->updatePubContents("Mr. Son scored at Tottenham");
	cout << endl;

	cout << "=========CNN Notify ===========" << endl;
	cnnPub->updatePubContents("New York city celebrates Christmas");
	cout << endl;

	cout << "=========DELETING [tomSub]===========" << endl;
	delete tomSub;
	cout << endl;


	cout << "=========DGIST Notify ===========" << endl;
	dgistPub->updatePubContents("Welcome New DGIST students");
	cout << endl;
	cout << "=========BBC Notify ===========" << endl;
	bbcPub->updatePubContents("Mr. Son scored at Tottenham");
	cout << endl;
	cout << "=========CNN Notify ===========" << endl;
	cnnPub->updatePubContents("New York city celebrates Christmas");
	cout << endl;

	cout << "=========Delete all others ===========" << endl;
	delete dgistPub;
	delete bbcPub;
	delete cnnPub;
	delete jennySub;
	delete kateSub;
	return 0;


}