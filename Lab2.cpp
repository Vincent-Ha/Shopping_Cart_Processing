#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <regex>
#include <bitset>
#include <vector>
#include <stack>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
using namespace std;

const static int HEX_BASE = 16;

class Read_File {
private:
	ifstream fin;
	string data;
public:
	Read_File() {};
	void readFile(string file_name, string file_path);
	string getData();
};

class ProductNode {
private:
	string name;
	string decodedName;
	double price;
	int quantity;
public:
	ProductNode() : name("N/A"), decodedName("N/A"), price(0), quantity(0) {};
	ProductNode(const ProductNode& p) : name(p.name), decodedName(p.decodedName), price(p.price), quantity(p.quantity) {};
	ProductNode& operator=(const ProductNode& p);
	void setName(string n);
	void setDecodedName(string dN);
	void setPrice(double p);
	void setQuantity(int q);
	string getName();
	string getDecodedName();
	double getPrice();
	int getQuantity();
};

class CartNode {
private:
	string cartName;
	queue <string> untranslatedItems;
	queue <ProductNode> translatedItems;
	double total;
public:
	CartNode() : cartName("N/A"), total(0) {};
	CartNode(const CartNode& c) : cartName(c.cartName), untranslatedItems(c.untranslatedItems), translatedItems(c.translatedItems), total(c.total) {};
	CartNode& operator=(const CartNode& c);
	void setCartName(string cN);
	void setUntranslatedItems(queue <string> uI);
	void setTranslatedItems(queue <ProductNode> tI);
	void setTotal();
	string getCartName();
	queue <string> getUntranslatedItems();
	queue <ProductNode> getTranslatedItems();
	double getTotal();
};

class CharacterNode {
private:
	char character;
	string symbology;
	bitset<24> binary;
public:
	CharacterNode() : character('\n'), symbology("N/A") {};
	void setCharacter(char c);
	void setSymbology(string s);
	void setBinary(bitset<24> b);
	char getCharacter();
	bitset<24> getBinary();
};

class ProductParser {
private:
	Read_File* productReader;
	string data;
	vector <ProductNode> inventory;
	void setData(string d);
public:
	ProductParser() : productReader(new Read_File), data("N/A") {};
	~ProductParser();
	void readFile(string file_name, string file_path);
	void parse();
	vector <ProductNode> getInventory();
};

class CartParser {
private:
	Read_File* cartReader;
	string data;
	vector <CartNode> shoppingCarts;
	void setData(string d);
public:
	CartParser() : cartReader(new Read_File), data("N/A") {};
	~CartParser();
	void readFile(string file_name, string file_path);
	void parse();
	vector <CartNode> getShoppingCarts();
};

class XMLParser {
private:
	Read_File* xmlReader;
	string data;
	queue <CharacterNode> dataCollection;
	void setData(string d);
	void grabAttributes(queue <string>& q, string s);
	void organizeQueueData(queue <string>& q);
public:
	XMLParser() : xmlReader(new Read_File), data("N/A") {};
	~XMLParser();
	void readFile(string file_name, string file_path);
	void parse();
	queue <CharacterNode> getDataCollection();
};

class Checkout {
private:
	CartParser* customers;
	XMLParser* barcodeData;
	ProductParser* productList;
	vector <ProductNode> inventoryList;
	vector<CartNode> shoppingCarts;
	queue <CharacterNode> barcodeDecoder;
	void setInventoryList(vector <ProductNode> iL);
	void setShoppingCarts(vector <CartNode> sC);
	void setBarcodeDecoder(queue <CharacterNode> bD);
	string stringToProductName(string n);
	queue <ProductNode> decodeItems(queue <string>& n);
	int binarySearch(int leftIndex, int rightIndex, string key);
public:
	Checkout() : customers(new CartParser), barcodeData(new XMLParser), productList(new ProductParser) {};
	~Checkout();
	void readFiles(string customer_file_name, string customer_file_path, string barcode_file_name, string barcode_file_path, string product_file_name, string product_file_path);
	void parseFiles();
	void processCarts();
	void printCarts();
};

class UserInterface {
private:
	Checkout* store;
public:
	UserInterface() : store(new Checkout) {};
	~UserInterface();
	void menu();
};

int main() {
	UserInterface uI;
	uI.menu();
	return 0;
}

void Read_File::readFile(string file_name, string file_path) {
	string name, content;
	string entireFile = "";
	fin.open(file_path);
	if (!fin.good()) {
		cerr << "Invalid File Path for the " + file_name + ". Please Reload With Proper File Path." << endl;
		exit(-92);
	}

	cout << "Reading data from the " + file_name + "." << endl;
	while (!fin.eof()) {
		getline(fin, content);
		entireFile += content;
		entireFile += "\n";
		content.clear();
	}
	fin.close();

	data = entireFile;
}

string Read_File::getData() {
	return data;
}

ProductNode& ProductNode::operator=(const ProductNode& p) {
	name = p.name;
	decodedName = p.decodedName;
	price = p.price;
	quantity = p.quantity;
	return *this;
}

void ProductNode::setName(string n) {
	name = n;
}

void ProductNode::setDecodedName(string dN) {
	regex pattern9(" ");
	regex pattern10("[&'()]");

	dN = regex_replace(dN, pattern9, "^");
	dN = regex_replace(dN, pattern10, "");
	for (int count = 0; count < dN.size(); count++)
		dN[count] = toupper(dN[count]);

	decodedName = dN;
}

void ProductNode::setPrice(double p) {
	price = p;
}

void ProductNode::setQuantity(int q) {
	quantity = q;
}

string ProductNode::getName() {
	return name;
}

string ProductNode::getDecodedName() {
	return decodedName;
}

double ProductNode::getPrice() {
	return price;
}

int ProductNode::getQuantity() {
	return quantity;
}

CartNode& CartNode::operator=(const CartNode& c) {
	cartName = c.cartName;
	untranslatedItems = c.untranslatedItems;
	translatedItems = c.translatedItems;
	total = c.total;
	return *this;
}

void CartNode::setCartName(string cN) {
	cartName = cN;
}

void CartNode::setUntranslatedItems(queue <string> uI) {
	untranslatedItems = uI;
}

void CartNode::setTranslatedItems(queue <ProductNode> tI) {
	translatedItems = tI;
}

void CartNode::setTotal() {
	queue <ProductNode> copy(translatedItems);
	while (!copy.empty()) {
		ProductNode top = copy.front();
		copy.pop();
		total += top.getPrice();
	}
}

string CartNode::getCartName() {
	return cartName;
}

queue <string> CartNode::getUntranslatedItems() {
	return untranslatedItems;
}

queue <ProductNode> CartNode::getTranslatedItems() {
	return translatedItems;
}

double CartNode::getTotal() {
	return total;
}

void CharacterNode::setCharacter(char c) {
	character = c;
}

void CharacterNode::setSymbology(string s) {
	symbology = s;
}

void CharacterNode::setBinary(bitset<24> b) {
	binary = b;
}

char CharacterNode::getCharacter() {
	return character;
}

bitset<24> CharacterNode::getBinary() {
	return binary;
}

void ProductParser::setData(string d) {
	data = d;
}

ProductParser::~ProductParser() {
	delete productReader;
}

void ProductParser::readFile(string file_name, string file_path) {
	productReader->readFile(file_name, file_path);
	setData(productReader->getData());
}

void ProductParser::parse() {
	cout << "Processing the ProductPrice.csv file." << endl;
	string tempName;
	regex pattern1("[^,\n]+");
	regex pattern2("Chicken, flaked");
	data = regex_replace(data, pattern2, "Chicken flaked");
	ProductNode temp;

	sregex_token_iterator end;
	for (sregex_token_iterator iter(data.begin(), data.end(), pattern1); iter != end; iter++) {
		temp.setName(static_cast<string>(*iter));
		temp.setDecodedName(static_cast<string>(*iter));
		iter++;
		temp.setPrice(stod(static_cast<string>(*iter)));
		iter++;
		temp.setQuantity(stoi(static_cast<string>(*iter)));
		inventory.push_back(temp);
	}
}

vector <ProductNode> ProductParser::getInventory() {
	return inventory;
}

void CartParser::setData(string d) {
	data = d;
}

CartParser::~CartParser() {
	delete cartReader;
}

void CartParser::readFile(string file_name, string file_path) {
	cartReader->readFile(file_name, file_path);
	setData(cartReader->getData());
}

void CartParser::parse() {
	cout << "Processing the Carts.csv file." << endl;
	regex pattern1("[^,\n]+");
	regex pattern3("Cart\\d+");
	sregex_token_iterator end;

	bool firstTime = true;
	CartNode tempCart;
	queue <string> tempQueue;
	for (sregex_token_iterator iter(data.begin(), data.end(), pattern1); iter != end; iter++) {
		if (regex_match(static_cast<string>(*iter), pattern3)) {
			if (!firstTime) {
				tempCart.setUntranslatedItems(tempQueue);
				queue <string>().swap(tempQueue);
				shoppingCarts.push_back(tempCart);
			}
			tempCart.setCartName(static_cast<string>(*iter));
			firstTime = false;
		}
		else {
			tempQueue.push(static_cast<string>(*iter));
		}
	}
	tempCart.setUntranslatedItems(tempQueue);
	shoppingCarts.push_back(tempCart);
}

vector <CartNode> CartParser::getShoppingCarts() {
	return shoppingCarts;
}

void XMLParser::setData(string d) {
	data = d;
}

void XMLParser::grabAttributes(queue <string>& q, string s) {
	regex pattern7("[^<>]+");
	regex pattern8("Hex");
	sregex_token_iterator iter2(s.begin(), s.end(), pattern7);

	if (regex_match(static_cast<string>(*iter2), pattern8))
		return;
	iter2++;
	q.push(static_cast<string>(*iter2));
}

void XMLParser::organizeQueueData(queue <string>& q) {
	CharacterNode temp;
	while (!q.empty()) {
		temp.setCharacter(q.front()[0]);
		q.pop();
		temp.setSymbology(q.front());
		q.pop();
		bitset <24> tempBitset(q.front());
		temp.setBinary(tempBitset);
		q.pop();
		dataCollection.push(temp);
	}
}

XMLParser::~XMLParser() {
	delete xmlReader;
}

void XMLParser::readFile(string file_name, string file_path) {
	xmlReader->readFile(file_name, file_path);
	setData(xmlReader->getData());
}

void XMLParser::parse() {
	cout << "Processing the Barcodes3of9.xml file." << endl;
	regex pattern4("<\\w+>.+|<\\w+>");
	regex pattern5("<\\w+>");
	regex pattern6("\/(?!<)");
	data = regex_replace(data, pattern6, "");

	sregex_token_iterator iter(data.begin(), data.end(), pattern4);
	stack <string> tags;
	queue <string> info;
	string temp;

	tags.push(static_cast<string>(*iter));
	iter++;
	while (!tags.empty()) {
		temp = static_cast<string>(*iter);
		if (regex_match(temp, pattern5)) {
			if (tags.top() == temp)
				tags.pop();
			else
				tags.push(temp);
		}
		else
			grabAttributes(info, temp);
		iter++;
	}
	organizeQueueData(info);
}

queue <CharacterNode> XMLParser::getDataCollection() {
	return dataCollection;
}

void Checkout::setInventoryList(vector <ProductNode> iL) {
	inventoryList = iL;
}

void Checkout::setShoppingCarts(vector <CartNode> sC) {
	shoppingCarts = sC;
}

void Checkout::setBarcodeDecoder(queue <CharacterNode> bD) {
	barcodeDecoder = bD;
}

string Checkout::stringToProductName(string n) {
	string binaryName = "";
	string nameOfProduct = "";
	string hex;
	queue <CharacterNode> barcodes;

	for (int count = 0; count < n.size(); count += 2) {
		char firstChar = n[count];
		char secondChar = n[count + 1];
		hex = "0x";
		hex += firstChar;
		hex += secondChar;
		bitset<8> charPart(stoi(hex, nullptr, HEX_BASE));
		binaryName += charPart.to_string();

		if ((count + 1) % 6 == 5) {
			bitset <24> character(binaryName);
			binaryName.clear();
			barcodes = barcodeDecoder;
			while (!barcodes.empty()) {
				if ((barcodes.front()).getBinary() == character) {
					nameOfProduct += (barcodes.front()).getCharacter();
					break;
				}
				barcodes.pop();
			}
		}
	}
	return nameOfProduct;
}

queue <ProductNode> Checkout::decodeItems(queue <string>& n) {
	queue <ProductNode> tempQueue;
	string tempString, nameOfProduct;
	ProductNode tempProduct;

	while (!n.empty()) {
		tempString = n.front();
		n.pop();
		nameOfProduct = stringToProductName(tempString);
		if (nameOfProduct == "INVENTORY" || nameOfProduct == "ITEM" || nameOfProduct == "10.74") {		
			tempProduct.setName(nameOfProduct);
			tempQueue.push(tempProduct);
			continue;
		}
			
		int index = binarySearch(0, inventoryList.size() - 1, nameOfProduct);

		if (index > -1) 
			tempQueue.push(inventoryList[index]);
		else 
			cout << "Error. Cannot find Product." << endl;
	}
	return tempQueue;
}

int Checkout::binarySearch(int leftIndex, int rightIndex, string key) {
	if (leftIndex > rightIndex)
		return -1;

	int middleIndex = (leftIndex + rightIndex) / 2;
	if (inventoryList[middleIndex].getDecodedName() == key)
		return middleIndex;
	if (inventoryList[middleIndex].getDecodedName() > key)
		return binarySearch(leftIndex, middleIndex - 1, key);
	return binarySearch(middleIndex + 1, rightIndex, key);
}

Checkout::~Checkout() {
	delete customers;
	delete barcodeData;
	delete productList;
}

void Checkout::readFiles(string customer_file_name, string customer_file_path, string barcode_file_name, string barcode_file_path, string product_file_name, string product_file_path) {
	cout << "Progress" << endl;
	cout << "--------" << endl;
	customers->readFile(customer_file_name, customer_file_path);
	barcodeData->readFile(barcode_file_name, barcode_file_path);
	productList->readFile(product_file_name, product_file_path);
}

void Checkout::parseFiles() {
	customers->parse();
	barcodeData->parse();
	productList->parse();
	setShoppingCarts(customers->getShoppingCarts());
	setBarcodeDecoder(barcodeData->getDataCollection());
	setInventoryList(productList->getInventory());
	sort(inventoryList.begin(), inventoryList.end(), [](ProductNode& a, ProductNode& b) { return a.getDecodedName() < b.getDecodedName(); });
}

void Checkout::processCarts() {
	cout << "Calculating Totals for Carts." << endl;
	for (int a = 0; a < shoppingCarts.size(); a++) {
		shoppingCarts[a].setTranslatedItems(decodeItems(shoppingCarts[a].getUntranslatedItems()));
		shoppingCarts[a].setTotal();
	}
	cout << endl;
}

void Checkout::printCarts() {
	cout << "Reciepts" << endl;
	cout << "--------" << endl;
	cout << endl;

	for (auto cart : shoppingCarts) {
		queue <ProductNode> tempQueue = cart.getTranslatedItems();
		cout << cart.getCartName() << endl;
		cout << "-------" << endl;
		while (!tempQueue.empty()) {
			if ((tempQueue.front()).getPrice() == 0) {
				tempQueue.pop();
				continue;
			}
			cout << left << setw(29) << (tempQueue.front()).getName() << fixed << setprecision(2) << setw(1) << "$" << right << setw(7) << (tempQueue.front()).getPrice() << endl;
			tempQueue.pop();
		}
		cout << "-------------------------------------" << endl;
		cout << left << setw(29) << "Total" << fixed << setprecision(2) << setw(1) << "$" << right << setw(7) << cart.getTotal() << endl;
		cout << endl;
	}
}

UserInterface::~UserInterface() {
	delete store;
}

void UserInterface::menu() {
	cout << "Welcome to the Cart Processing Application!" << endl;
	cout << "-------------------------------------------" << endl;
	cout << endl;
	store->readFiles("Carts.csv", "c:/Users/Vincent Ha/Downloads/Carts.csv", "Barcodes3of9.xml", "c:/Users/Vincent Ha/Downloads/Barcodes3of9.xml", "ProductPrice.csv", "c:/Users/Vincent Ha/Downloads/ProductPrice.csv");
	store->parseFiles();
	store->processCarts();
	store->printCarts();
}