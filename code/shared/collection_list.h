// https://github.com/webcoyote/coho/blob/master/Base/List.h
// https://www.codeofhonor.com/blog/avoiding-game-crashes-related-to-linked-lists
// https://gpfault.net/posts/intrusive-lists-doom3.txt.html
// http://fabiensanglard.net/doom3_documentation/DOOM-3-BFG-Technical-Note.pdf

/*
Intrusive linked list

Link is just a pointer into its parent object
List converts object to link and vice versa using offsetof()

I can't but see analogies here:
* lists and links
* kneeting needles and stitches
* strings and beads
*/

struct Link {
	Link *prev;
	Link *next;

	Link();
	~Link();

	void InsertBefore(Link *link);
	void InsertAfter(Link *link);

	void Unlink();
};

#define LIST_DECLARE(T, link, name) List<T> name(offsetof(T, link))
template<typename T>
struct List {
	Link base_link;
	size_t link_offset;

	List(size_t offset);

	T *LinkToNode(Link *link);
	Link *NodeToLink(T *node);

	void InsertHead(T *node);
	void InsertTail(T *node);

	void InsertBefore(T *node, T *base_node);
	void InsertAfter(T *node, T *base_node);

	void Unlink(T *node);
	
	T *Head();
	T *Tail();
	const T *Head() const;
	const T *Tail() const;
	
	T *Next(T *node);
	T *Prev(T *node);
	const T *Next(const T *node) const;
	const T *Prev(const T *node) const;
};

//
//
//

Link::Link()
{
	prev = this;
	next = this;
}

Link::~Link()
{
	Unlink();
}

void Link::Unlink()
{
	prev->next = next;
	next->prev = prev;
	prev = this;
	next = this;
}

void Link::InsertBefore(Link *link)
{ // [prev] x [this]
	link->prev = prev; // x to [prev]
	link->next = this; // x to [this]
	
	prev->next = link; // [prev] to x
	prev       = link; // [this] to x
}

void Link::InsertAfter(Link *link)
{ // [this] x [next]
	link->next = next; // x to [next]
	link->prev = this; // x to [this]
	
	next->prev = link; // [next] to x
	next       = link; // [this] to x
}

//
//
//

template<typename T>
List<T>::List(size_t offset)
	: base_link()
	, link_offset(offset)
{ }

template<typename T>
T *List<T>::LinkToNode(Link *link)
{
	if (link == &base_link) { return NULL; }
	return (T *)((size_t)link - link_offset);
}

template<typename T>
Link *List<T>::NodeToLink(T *node)
{
	return (Link *)((size_t)node + link_offset);
}

template<typename T>
void List<T>::InsertHead(T *node)
{
	auto link = NodeToLink(node);
	base_link.InsertBefore(link);
}

template<typename T>
void List<T>::InsertTail(T *node)
{
	auto link = NodeToLink(node);
	base_link.InsertAfter(link);
}

template<typename T>
void List<T>::InsertBefore(T *node, T *base_node)
{
	auto link = NodeToLink(node);
	auto base_node_link = NodeToLink(base_node);
	base_node_link->InsertBefore(link);
}

template<typename T>
void List<T>::InsertAfter(T *node, T *base_node)
{
	auto link = NodeToLink(node);
	auto base_node_link = NodeToLink(base_node);
	base_node_link->InsertAfter(link);
}

template<typename T>
void List<T>::Unlink(T *node)
{
	auto link = NodeToLink(node);
	link->Unlink();
}

template<typename T>
T *List<T>::Head()
{
	return LinkToNode(base_link.next);
}

template<typename T>
T *List<T>::Tail()
{
	return LinkToNode(base_link.prev);
}

template<typename T>
const T *List<T>::Head() const
{
	return LinkToNode(base_link.next);
}

template<typename T>
const T *List<T>::Tail() const
{
	return LinkToNode(base_link.prev);
}

template<typename T>
T *List<T>::Next(T *node)
{
	auto link = NodeToLink(node);
	return LinkToNode(link->next);
}

template<typename T>
T *List<T>::Prev(T *node)
{
	auto link = NodeToLink(node);
	return LinkToNode(link->prev);
}

template<typename T>
const T *List<T>::Next(const T *node) const
{
	auto link = NodeToLink(node);
	return LinkToNode(link->next);
}

template<typename T>
const T *List<T>::Prev(const T *node) const
{
	auto link = NodeToLink(node);
	return LinkToNode(link->prev);
}
