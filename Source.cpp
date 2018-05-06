#include <iostream>
#include <string>
#include <fstream>
#include <queue>
using namespace std;
// linija 588
struct Proces {
	string ime;
	int t_to_complete, max_w_time, w_time, exec_time;
	Proces(string ime, int t_to_complete, int max_w_time) :ime(ime), t_to_complete(t_to_complete), max_w_time(max_w_time) {
		w_time = 0;
		exec_time = 0;

	}
};

struct Node {
	Node *child[4], *parent;
	Proces *keys[3];

	Node(Proces *proces, Node *parent = nullptr) : parent(parent) {
		for (int i = 0; i < 4; i++)
			child[i] = nullptr;
		keys[1] = proces;
		keys[0] = keys[2] = nullptr;
	}
	int NumOfChild() const {
		int x = 0;
		for (int i = 0; i < 4; i++)
			if (child[i])
				x++;
		return x;
	}

	bool isLeaf() const {
		return (NumOfChild() == 0);
	}

	int NumOfElem() const {
		int x = 0;
		if (keys[0])
			x++;
		if (keys[1])
			x++;
		if (keys[2])
			x++;
		return x;
	}
};

class Queue {
public:
	Queue() {
		left = right = 0;
	}

	bool Full() const {
		return right >= 100;
	}

	bool Empty() const {
		return left == right;
	}

	void Insert(Node *p, int l) {
		if (Full())
			exit(1);
		level[right] = l;
		q[right++] = p;
	}

	Node *Delete() {
		if (Empty())
			return nullptr;
		return q[left++];
	}
	int DeleteLevel() {
		if (Empty())
			return 0;
		return level[left];
	}
private:
	Node *q[100];
	int level[100];
	int left, right;
};

class Tree {
public:
	Tree() {
		root = nullptr;
	}

	Node *nadjiMesto(Proces *proces) const {
		Node *tek = root, *pret = nullptr;
		bool found = true;
		while (found && tek) {
			found = false;
			pret = tek;
			for (int i = 0; i < 3; i++)
				if (tek->keys[i] && proces->w_time < tek->keys[i]->w_time) {
					found = true;
					tek = tek->child[i];
					break;
				}

			if (!found) {
				for (int i = 2; i >= 0; i--)
					if (tek->keys[i]) {
						tek = tek->child[i + 1];
						found = true;
						break;
					}
			}
		}


		return pret;
	}

	friend ostream &operator<<(ostream &os, Tree t) {
		Queue q;
		Node *tek = t.root;;
		os << "__________________________________________\n\n\n-----------------------\nNivo 0\n------------------------\n";
		int last_lvl = 0, lvl, i;

		if (tek)
			q.Insert(tek, 0);

		while (!q.Empty()) {
			lvl = q.DeleteLevel();
			tek = q.Delete();
			if (lvl != last_lvl)
				os << "\n-----------------------\nNivo " << lvl << "\n-----------------------\n\n";
			for (i = 0; i < 3; i++)
				if (tek->keys[i]) {
					os << " -|- " << tek->keys[i]->ime << ": wt: " << tek->keys[i]->w_time << " et:" << tek->keys[i]->exec_time
						<< " ttc: " << tek->keys[i]->t_to_complete << " mwt: " << tek->keys[i]->max_w_time << " -|- ";
				}
				else
					os << " -|- NULL -|- ";
			if (tek->parent)
				os << "PARENT: " << tek->parent->keys[1]->ime;

			for (i = 0; i < 4; i++)
				if (tek->child[i])
					q.Insert(tek->child[i], lvl + 1);
			last_lvl = lvl;
			os << endl;
		}

		return os;
	}

	void ubaci(Proces *proces, Node *&pret, Node *right = nullptr, Node *left = nullptr) {
		int i = 0, j = 0;
		bool ubacen = false;
		while (!ubacen) {
			if (!pret) {
				root = new Node(proces);
				ubacen = true;
			}
			// ubacivanje ako je stablo prazno

			else if (pret->NumOfElem() == 3) {
				Node *q = new Node(pret->keys[2], pret->parent);
				Proces *mid = pret->keys[1];

				q->child[2] = pret->child[3];
				if (pret->child[3])
					pret->child[3]->parent = q;
				pret->keys[1] = pret->keys[0];
				pret->child[3] = pret->child[2];
				pret->child[2] = pret->child[1];
				pret->child[1] = pret->child[0];
				pret->child[0] = nullptr;
				pret->keys[2] = pret->keys[0] = nullptr;

				if (proces->w_time < pret->keys[1]->w_time) {
					pret->keys[0] = proces;
					pret->child[0] = left;
					pret->child[1] = right;
					if (left)
						left->parent = pret;
					if (right)
						right->parent = pret;
					if (!pret->keys[2]) {
						q->child[1] = pret->child[3];
						pret->child[3] = nullptr;
						if (q->child[1])
							q->child[1]->parent = q;
					}
				}
				else if (proces->w_time < mid->w_time) {
					pret->keys[2] = proces;
					q->child[1] = pret->child[3];
					if (pret->child[3])
						pret->child[3]->parent = q;
					pret->child[3] = right;
					pret->child[2] = left;
					if (left)
						left->parent = pret;
					if (right)
						right->parent = pret;
				}
				else if (proces->w_time < q->keys[1]->w_time) {
					q->keys[0] = proces;
					q->child[0] = left;
					q->child[1] = right;
					if (left)
						left->parent = q;
					if (right)
						right->parent = q;
				}
				else {
					q->child[2] = left;
					q->child[3] = right;
					q->child[1] = pret->child[3];
					if (pret->child[3])
						pret->child[3]->parent = q;
					if (left)
						left->parent = q;
					if (right)
						right->parent = q;
					pret->child[3] = nullptr;
					q->keys[2] = proces;
				}
				if (pret == root) {
					Node *newRoot = new Node(mid);
					newRoot->child[1] = pret;
					newRoot->child[2] = q;
					pret->parent = newRoot;
					q->parent = newRoot;
					root = newRoot;
					ubacen = true;
				}
				else {
					//ubaci(mid, pret->parent, q, pret);
					proces = mid;
					right = q;
					left = pret;
					pret = pret->parent;
				}
			}
			// ubacivanje ako list jeste pun

			else {
				if (pret->NumOfElem() == 1) {
					if (proces->w_time >= pret->keys[1]->w_time) {
						if (right && right->keys[1]->w_time == proces->w_time) {
							pret->keys[2] = proces;
							pret->child[3] = pret->child[2];
							pret->child[2] = right;
							if (right)
								right->parent = pret;
						}
						else {
							pret->keys[2] = proces;
							//	pret->child[2] = left;
							pret->child[3] = right;
							if (left)
								left->parent = pret;
							if (right)
								right->parent = pret;
						}
					}
					/*else if (proces->w_time == pret->keys[1]->w_time) {
						pret->keys[2] = proces;
						pret->child[3] = pret->child[2];
						pret->child[2] = right;
						pret->child[1] = left;
						if (left)
							left->parent = pret;
						if (right)
							right->parent = pret;
					}*/
					else {
						pret->child[0] = left;
						pret->child[1] = right;
						pret->keys[0] = proces;
						if (left)
							left->parent = pret;
						if (right)
							right->parent = pret;
					}
				}

				//najveci
				else if ((pret->keys[2] && proces->w_time >= pret->keys[2]->w_time) || (pret->keys[0] && proces->w_time >= pret->keys[1]->w_time)) {
					if (pret->keys[0]) {
						pret->keys[2] = proces;
						pret->child[2] = left;
						pret->child[3] = right;
					}
					else {
						pret->keys[0] = pret->keys[1];
						pret->keys[1] = pret->keys[2];
						pret->keys[2] = proces;
						pret->child[0] = pret->child[1];
						pret->child[1] = pret->child[2];
						pret->child[2] = left;
						pret->child[3] = right; // childofproces
					}
				}

				//srednji;
				else if ((pret->keys[2] && (proces->w_time >= pret->keys[1]->w_time && proces->w_time < pret->keys[2]->w_time))
					|| (pret->keys[0] && (proces->w_time < pret->keys[1]->w_time && proces->w_time >= pret->keys[0]->w_time)))
				{
					if (pret->keys[2]) {
						pret->keys[0] = pret->keys[1];
						pret->keys[1] = proces;
						pret->child[0] = pret->child[1];
						pret->child[1] = pret->child[2];
						pret->child[2] = right; // child of proces
					}
					else {
						if (pret->child[0] == left){
							pret->child[3] = pret->child[2];
							pret->child[2] = pret->child[1];
							pret->child[1] = right;
							pret->keys[2] = pret->keys[1];
							pret->keys[1] = proces;
							if (right)
								right->parent = pret;
						}
						else {
							pret->keys[2] = pret->keys[1];
							pret->keys[1] = proces;
							pret->child[3] = pret->child[2];
							pret->child[1] = left;
							pret->child[2] = right; // child of proces
						}
						if (left)
							left->parent = pret;
						if (right)
							right->parent = pret;
					//	pret->child[3] = pret->child[2];
					}
				}

				//najmanji;
				else if (pret->keys[2]) {
					pret->keys[0] = proces;
					pret->child[0] = left;
					pret->child[1] = right;
				}
				else {
					pret->keys[2] = pret->keys[1];
					pret->keys[1] = pret->keys[0];
					pret->keys[0] = proces;
					pret->child[3] = pret->child[2];
					pret->child[2] = pret->child[1];
					pret->child[1] = right;
					pret->child[0] = left; // child of proces
					if (pret->child[1])
						pret->child[1]->parent = pret;
					if (pret->child[0])
						pret->child[0]->parent = pret;
				}
				ubacen = true;
			}
			// ubacivanje ako list nije pun
		}
	}

	Tree &dodaj(Proces *proces) {
		Node *pret = nadjiMesto(proces);
		ubaci(proces, pret);
		return *this;
	}

	Node *findSucc(Node *p, int ind) {
		Node *q = p->child[ind + 1];
		Node *tek = q;
		while (!tek->isLeaf()) {
			for (int i = 0; i < 4; i++) {
				if (tek->child[i]) {
					tek = tek->child[i];
					break;
				}
			}
		}
		return tek;
	}

	Proces *brisiNajlevlji() {
		if (root == nullptr)
			return nullptr;
		int i;
		Proces min("", 0, 0);
		min.w_time = -1;
		Node *tek = nadjiMesto(&min);
		if (tek == nullptr)
			return nullptr;
		for (i = 0; i < 3 && !tek->keys[i]; i++);
		return obrisi(tek, i);
	}

	void obrisiCelo() {

		root = nullptr;
	}

	void simulate() {
		ofstream izlaz;
		int time_slice, new_time_slice, br = 0;
		queue <Proces*> redp;
		queue <Node*> q;
		Proces *p, *p2;

		izlaz.open("izlaz.txt");

		cout << endl << endl << "Unesite inkrement vremena: ";
		cin >> time_slice;

		while (root) {

			new_time_slice = time_slice;
		//	cout << *this;

			p = brisiNajlevlji();
			//cout << *this;
			if (p->exec_time + time_slice >= p->t_to_complete)
				new_time_slice = p->t_to_complete - p->exec_time;

			Node *tek = root;

			izlaz << "_____________________" << endl << ++br << ") PROCES: " << p->ime << endl << "_____________________" << endl;
			cout << "_____________________" << endl << br << ") PROCES: " << p->ime << endl << "_____________________" << endl;

			if (tek)
				q.push(tek);

			while (!q.empty()) {
				tek = q.front();
				q.pop();
				for (int i = 0; i < 3; i++)
					if (tek->keys[i]) {
						tek->keys[i]->w_time += new_time_slice;
						if (tek->keys[i]->w_time > tek->keys[i]->max_w_time)
							redp.push(tek->keys[i]);
					}
				for (int i = 0; i < 4; i++)
					if (tek->child[i])
						q.push(tek->child[i]);
			}

			while (!redp.empty()) {
				bool found = false;
				p2 = redp.front();
				redp.pop();
				tek = root;
				//cout << *this;
				if (tek)
					q.push(tek);
				while (!q.empty() && !found) {
					tek = q.front();
					q.pop();
					for (int i = 0; i < 3 && !found; i++)
						if (tek->keys[i] == p2) {
							p2->w_time %= p2->max_w_time;
							p2 = obrisi(tek, i);// dodavanje notepad++
						//	cout << *this;
							dodaj(p2);
							//cout << *this;
							found = true;
						}
					for (int i = 0; i < 4; i++)
						if (tek->child[i])
							q.push(tek->child[i]);
				}
				while (!q.empty())
					q.pop();
			}

			if (p->exec_time + time_slice >= p->t_to_complete) {
				cout << endl << endl << "Izvrsen proces: " << p->ime << " u koraku " << br << endl << endl;
				izlaz << endl << endl << "Izvrsen proces: " << p->ime << " u koraku " << br << endl << endl;
				delete p;
			}
			else {
				//cout << *this;
				p->exec_time += time_slice;
				p->w_time += time_slice;
				if (p->w_time > p->max_w_time)
					p->w_time %= p->max_w_time;
				//cout << *this;
				dodaj(p);
				//cout << *this;
			}
			izlaz << *this;
			//izlaz << *this;
			//cout << *this;
			// ispisivanje elemenata u datoteku
		}
	}

	Proces *obrisi(Node *tek, int ind, Node *child = nullptr) {
		Proces *p = tek->keys[ind];
		Node *parent = tek->parent, *brother = nullptr;
		bool obrisan = false;
		int redni;

		if (!tek->isLeaf()) {
			Node *zamena = findSucc(tek, ind);
			for (int i = 0; i < 3; i++) {
				if (zamena->keys[i]) {
					Proces *tmp = zamena->keys[i];
					zamena->keys[i] = tek->keys[ind];
					tek->keys[ind] = tmp;
					ind = i;
					tek = zamena;
					parent = tek->parent;
					break;
				}
			}
			//cout << *this;
		}

		while (!obrisan) {
			if (tek->NumOfElem() == 3) {
				if (ind == 2 || ind == 0)
					tek->keys[ind] = nullptr;
				else {
					tek->keys[1] = tek->keys[0];
					tek->keys[0] = nullptr;
				}
				obrisan = true;
			}
			// brisanje iz punog cvora

			else if (tek->NumOfElem() == 2) {
				if (ind == 2 || ind == 0)
					tek->keys[ind] = nullptr;
				else {
					if (tek->keys[2]) {
						tek->keys[1] = tek->keys[2];
						tek->keys[2] = nullptr;
					}
					else {
						tek->keys[1] = tek->keys[0];
						tek->keys[0] = nullptr;
					}
				}
				obrisan = true;
			}
			// brisanje iz cvora koji ima 2 kljuca

			else {
				if (tek == root) {
					tek->keys[ind] = nullptr;
					obrisan = true;
				}
				else {
					for (int i = 0; i < 4; i++)
						if (parent->child[i] == tek)
							redni = i;

					if (redni <= 2 && parent->child[redni + 1] && parent->child[redni + 1]->NumOfElem() > 1) {
						brother = parent->child[redni + 1];
						tek->keys[ind] = parent->keys[redni];
						tek->child[ind] = child;
						if (child)
							child->parent = tek;

						for (int i = 0; i < 3; i++)
							if (brother->keys[i]) {
								parent->keys[redni] = brother->keys[i];
								if (i == 1) {
									tek->child[ind + 1] = brother->child[1];
									if (brother->child[1])
										brother->child[1]->parent = tek;

									brother->keys[1] = brother->keys[2];
									brother->child[1] = brother->child[2];
									brother->child[2] = brother->child[3];

									brother->keys[2] = nullptr;
									brother->child[3] = nullptr;
								}
								else {
									tek->child[ind + 1] = brother->child[0];
									if (brother->child[0])
										brother->child[0]->parent = tek;

									brother->keys[0] = nullptr;
									brother->child[0] = nullptr;
								}
								break;
							}
						obrisan = true;
					}
					// preuzimanje od desnog brata

					else if (redni >= 1 && parent->child[redni - 1] && parent->child[redni - 1]->NumOfElem() > 1) {
						brother = parent->child[redni - 1];
						tek->keys[ind] = parent->keys[redni - 1];
						tek->child[ind + 1] = child;
						if (child)
							child->parent = tek;
						for (int i = 2; i >= 0; i--)
							if (brother->keys[i]) {
								parent->keys[redni - 1] = brother->keys[i];
								tek->child[ind] = brother->child[i + 1];
								if (brother->child[i + 1])
									brother->child[i + 1]->parent = tek;
								brother->child[i + 1] = nullptr;
								if (i == 1) {
									brother->keys[1] = brother->keys[0];
									brother->child[2] = brother->child[1];
									brother->child[1] = brother->child[0];
									brother->child[0] = nullptr;
									brother->keys[0] = nullptr;
								}
								else
									brother->keys[2] = nullptr;
								obrisan = true;
								break;
							}
						obrisan = true;
					}
					// preuzimanje od levog brata

					else {
						bool bratLevo = false;
						if (redni == 3) {
							brother = parent->child[2];
							bratLevo = true;
						}
						else if (redni == 0)
							brother = parent->child[1];
						else {
							if (parent->child[redni + 1])
								brother = parent->child[redni + 1];
							else {
								brother = parent->child[redni - 1];
								bratLevo = true;
							}
						}
						if (bratLevo) {
							int i = redni - 1;// greska
							if (parent->keys[i]) {
								if (brother->keys[1]->w_time > parent->keys[i]->w_time) {
									brother->keys[2] = brother->keys[1];
									brother->child[3] = brother->child[2];
									brother->child[2] = brother->child[1];
									brother->child[1] = child;
									parent->child[i] = nullptr;
									if (child)
										child->parent = brother;
								}
								else {
									brother->keys[0] = brother->keys[1];
									brother->child[0] = brother->child[1];
									brother->child[1] = brother->child[2];
									brother->child[2] = child;
									if (child)
										child->parent = brother;
									parent->child[i + 1] = nullptr;
								}
								brother->keys[1] = parent->keys[i];
								parent->keys[i] = nullptr;
								if (i == 1) {
									parent->keys[1] = parent->keys[0];
									parent->keys[0] = nullptr;
									parent->child[2] = parent->child[1];
									parent->child[1] = parent->child[0];
									parent->child[0] = nullptr;
								}
								//break;
							}
						}


						else {
							//for (int i = 0; i < 3; i++)
							int i = redni;
								if (parent->keys[i]) {
									if (brother->keys[1]->w_time >= parent->keys[i]->w_time) {
										brother->keys[2] = brother->keys[1];
										brother->child[3] = brother->child[2];
										brother->child[2] = brother->child[1];
										brother->child[1] = child;
										parent->child[i] = nullptr;
										if (child)
											child->parent = brother;
									}
									else {
										brother->keys[0] = brother->keys[1];
										brother->child[0] = brother->child[1];
										brother->child[1] = brother->child[2];
										parent->child[i + 1] = nullptr;
									}
									brother->keys[1] = parent->keys[i];
									parent->keys[i] = nullptr;
									if (i == 1) {
										if (parent->keys[2]) {
											parent->keys[1] = parent->keys[2];
											parent->keys[2] = nullptr;
											parent->child[1] = parent->child[2];
											parent->child[2] = parent->child[3];
											parent->child[3] = nullptr;
										}
										else {
											parent->keys[1] = parent->keys[0];
											parent->keys[0] = nullptr;
											parent->child[2] = brother;
											parent->child[1] = parent->child[0];
											parent->child[0] = nullptr;
										}
									}
									else if (i == 2) {
										parent->child[2] = parent->child[3];
										parent->child[3] = nullptr;
									}
									//break;
								}
						}
						if (parent->NumOfElem() == 0 && parent == root) {
							root = brother;
							brother->parent = nullptr;
							tek->parent = nullptr;
							obrisan = true;
						}
						else if (parent->NumOfElem() == 0) {
							child = brother;
							tek = parent;
							ind = 1;
							parent = tek->parent;
						}
						else
							obrisan = true;
					}
					// nemoguce preuzimanje
				}
			}
			//brisanje iz cvora koji ima 1 kljuc

		}
		if (root->NumOfElem() == 0)
			root = nullptr;
		return p;
	}

//private:
	Node *root;
};

void dodajProces(Tree &t) {
	string ime;
	int max_w_time, t_to_complete;
	cout << "Ime, vreme potrebno da se proces izvrsi, maksimalno vreme cekanja: ";
	cin >> ime >> t_to_complete >> max_w_time;
	Proces *p = new Proces(ime, t_to_complete, max_w_time);
	t.dodaj(p);
}

void ucitajIzDat(Tree &t) {
	ifstream dat;
	string ime;
	int max_w_time = 0, t_to_complete = 0;


	dat.open("veliki.txt");

	if (!dat.is_open()) {
		cout << endl << endl << endl << "DATOTEKA NE POSTOJI" << endl << endl;
		system("pause");
		exit(-1);
	}


	while (!dat.eof()) {
		dat >> ime;
		dat >> t_to_complete;
		dat >> max_w_time;
		Proces *p = new Proces(ime, t_to_complete, max_w_time);
		t.dodaj(p);
	}

	dat.close();

}


int main() {
	Tree t;
	int opcija;
	bool izlaz = false;
	while (!izlaz) {
		cout << endl << endl << "-------------MENI-------------" << endl << endl << endl << endl << endl
			<< "1) Dodaj proces u stablo" << endl
			<< "2) Procitaj iz datoteke" << endl
			<< "3) Simulacija" << endl
			<< "4) Obrisi stablo" << endl
			<< "5) Ispis stabla" << endl
			<< "6) Izlaz iz programa" << endl << endl
			<< "Odaberite: ";
		cin >> opcija;
		switch (opcija) {
		case 1:
			dodajProces(t);
			break;
		case 2:
			ucitajIzDat(t);
			break;
		case 3:
			t.simulate();
			break;
		case 4:
			t.obrisiCelo();
			break;
		case 5:
			cout << t;
			break;
		case 6:
			izlaz = true;
			break;
		}


	}
	return 0;
}