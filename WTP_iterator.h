/*
 * WTP struct to keep a record of all WTPs
 */

struct WTP_connected {
	int WTP_id;
	int socket;
	char* macaddr;
	char* ipaddr;
	int port;
	char* WTP_name;
	WTP_connected *next;
};

struct WTP_connected *g_WTP_head = (struct WTP_connected*) NULL;
struct WTP_connected *g_WTP_tail = (struct WTP_connected*) NULL;
struct WTP_connected *initiate_WTP(int WTP_id, int socket, char* macaddr,
		char* ipaddr, int port, char* WTP_name);
void add_WTP(struct WTP_connected *newnode);
void insertNode(struct WTP_connected *newnode);
void deleteNode(struct WTP_connected *ptr);
void displayNode(struct WTP_connected *ptr);
void displayList(struct WTP_connected *ptr);
struct WTP_connected *search_node_by_socket(int socket);
void update_WTP_ids();

// Initiate
struct WTP_connected *initiate_WTP(int WTP_id, int socket, char* macaddr,
		char* ipaddr, int port, char* WTP_name) {
	struct WTP_connected *ptr = new WTP_connected;

	if (ptr == NULL) {
		return static_cast<struct WTP_connected *>(NULL);
	} else {
		ptr->WTP_id = WTP_id;
		ptr->socket = socket;
		ptr->macaddr = macaddr;
		ptr->ipaddr = ipaddr;
		ptr->port = port;
		ptr->WTP_name = WTP_name;
		return ptr;
	}
}

// add WTP
void add_WTP(struct WTP_connected *newnode) {
	//if there is no node, put it to head
	if (g_WTP_head == NULL) {
		g_WTP_head = newnode;
		g_WTP_tail = newnode;
	}

	g_WTP_tail->next = newnode;
	newnode->next = NULL;
	g_WTP_tail = newnode;
}

// insert WTP in between
// or insert at the end
void insertNode(struct WTP_connected *newnode) {
	struct WTP_connected *temp, *prev;

	//if its the first node to be added
	if (g_WTP_head == NULL) {
		add_WTP(newnode);
		update_WTP_ids();
		return;
	}

	temp = g_WTP_head;

	//check whether sockets line up properly
	while (temp->socket < newnode->socket) {
		temp = temp->next;
		if (temp == NULL)
			break;
	}

	if (temp == g_WTP_head) {
		newnode->next = g_WTP_head;
		g_WTP_head = newnode;
	} else {
		prev = g_WTP_head;
		while (prev->next != temp) {
			prev = prev->next;
		}
		prev->next = newnode;
		newnode->next = temp;
		if (g_WTP_tail == prev)
			g_WTP_tail = newnode;
	}
	update_WTP_ids();
}

void deleteNode(struct WTP_connected *ptr) {
	struct WTP_connected *temp, *prev;
	temp = ptr;
	prev = g_WTP_head;

	if (temp == prev) {
		g_WTP_head = g_WTP_head->next;
		if (g_WTP_tail == temp)
			g_WTP_tail = g_WTP_tail->next;
		delete temp;
	} else {
		while (prev->next != temp) {       // move prev to the node before
			prev = prev->next;              // the one to be deleted
		}
		prev->next = temp->next;            // link previous node to next
		if (g_WTP_tail == temp)                  // if this was the end node,
			g_WTP_tail = prev;                    // then reset the end pointer
		delete temp;
	}

}

void displayNode(struct WTP_connected *ptr) {
	printf(
			"WTP_id:%d, socket:%d, MAC-Addr:%s, IP-Addr:%s, Port:%d, WTP-name:%s\n",
			ptr->WTP_id, ptr->socket, ptr->macaddr, ptr->ipaddr, ptr->port,
			ptr->WTP_name);
}

void displayList(struct WTP_connected *ptr) {
	while (ptr != NULL) {
		displayNode(ptr);
		ptr = ptr->next;
	}
}

//search node
struct WTP_connected *search_node_by_socket(int socket) {
	struct WTP_connected *prev, *matched;
	prev = g_WTP_head;
	int found_socket = 0;
	while (prev != NULL) {
		if (prev->socket != socket) {
			printf("search for socket %d not matched with WTP-ID %d\n",
					prev->socket, prev->WTP_id);
			prev = prev->next;
		} else {
			printf("search for socket %d matched with WTP-ID %d\n",
					prev->socket, prev->WTP_id);
			found_socket++;
			matched = prev;
			prev = prev->next;
		}
	}

	if (found_socket > 1) {
		//multiple WTP with same socket
		perror(
				"Should not reach here!! Duplicate/multiple WTP with same socket!");
		return 0;
	} else if (found_socket == 0) {
		//no WTP found
		return static_cast<struct WTP_connected *>(NULL);
	}

	return matched;

}

void update_WTP_ids() {
	int id_counter = 0;
	struct WTP_connected *temp;
	temp = g_WTP_head;
	while (temp != NULL) {
		temp->WTP_id = id_counter;
		id_counter++;
		temp = temp->next;
	}
}
