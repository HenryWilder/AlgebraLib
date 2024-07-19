#pragma once

template<class T>
using Comparer = int(*)(const T& a, const T& b);

template<class T>
using Combiner = T(*)(const T& a, const T& b);

template<class T, class P>
using SimplificationStep = void(*)(T& x, P& persistent);

template<class T, class P, size_t N>
using SimplificationSteps = SimplificationStep<T, P>[N];

template<class T>
using ContributionPred = bool(*)(const T&);

template<
    class T,
    Comparer<T> Compare,
    Combiner<T> Combine,
    size_t NUM_SIMPLIFICATION_STEPS,
    class SimplificationMemory,
    SimplificationMemory SIMPLIFY_MEMORY_INIT,
    SimplificationSteps<T, SimplificationMemory, NUM_SIMPLIFICATION_STEPS> Simplify,
    ContributionPred<T> IsContributing
>
class SimplifyingSet
{
private:
    struct TreeNode
    {
        T value;
        TreeNode* children[2] = { nullptr, nullptr };

        TreeNode(const T& value) : value(value) { }
    };

    struct StackNode
    {
        TreeNode& value;
        StackNode* prev;

        StackNode(TreeNode& value, StackNode* prev)
            : value(value), prev(prev) { }
    };

    class Stack
    {
    public:
        ~Stack( )
        {
            while (tail)
            {
                StackNode* temp = tail;
                tail = tail->prev;
                delete temp;
            }
        }

        void Push(TreeNode& value)
        {
            tail = new StackNode(value, tail);
        }

        TreeNode* Top( ) const
        {
            return tail ? tail->value : nullptr;
        }

        TreeNode* Pop( )
        {
            if (tail)
            {
                StackNode* temp = tail;
                tail = tail.prev;
                TreeNode& top = temp->value;
                delete temp;
                return &top;
            }
            return nullptr;
        }

    private:
        StackNode* tail = nullptr;
    };

public:
    SimplifyingSet( ) { }

    SimplifyingSet(const T dataV[ ], size_t dataC)
    {
        Insert(dataV, dataC);
    }

    bool IsEmpty( ) const
    {
        return !root;
    }

    bool IsSingular( ) const
    {
        return !!root && !root->children[0] && !root->children[1];
    }

    void Insert(const T dataV[ ], size_t dataC)
    {

    }

    template<class T2, T(*Combinator)(const T&, const T2&)>
    void Combine(const T2& with)
    {

    }

    template<class T2, T(*Combinator)(const T&, const T2&)>
    void Combine(const T2 withV[ ], size_t withC)
    {

    }

    template<class T2, T*(*Combinator)(const T&, const T2[ ], size_t)>
    void Combine(const T2 withV[ ], size_t withC)
    {

    }

    template<T(*Combinator)(const T&, const T&)>
    void Combine(const T& with)
    {

    }

    const T* Tip(bool greater) const
    {
        if (!root) return nullptr;

        TreeNode* node = root;
        while (true)
        {
            TreeNode* child = node.children[greater ? 1 : 0];
            if (child) node = child;
            else return node->value;
        }
    }

    const T* Leading( ) const
    {
        return Tip(false);
    }

    const T* Trailing( ) const
    {
        return Tip(true);
    }

    class SimplifyingSetIterator
    {
    private:
        SimplifyingSetIterator(const SimplifyingSet& source, Stack& stack)
            : source(source), stack(stack)
        { }

    public:
        ~SimplifyingSetIterator()
        {
            delete &stack;
        }

        SimplifyingSetIterator& operator++( )
        {
            const TreeNode* top = stack.Top( );

            const TreeNode* right = top->children[1];
            if (right)
            {
                stack.Push(right);
                const TreeNode* left;
                while (true)
                {
                    left = right->children[0];
                    if (left)
                    {
                        stack.Push(left);
                    }
                    else
                    {
                        return;
                    }
                }
            }
        }

        SimplifyingSetIterator& operator--( )
        {
            // todo
        }

        const T* operator*( ) const
        {
            const TreeNode* top = stack.Top( );
            return top ? top->value : nullptr;
        }

        T* operator*( )
        {
            TreeNode* top = stack.Top( );
            return top ? top->value : nullptr;
        }

        bool operator!=(const SimplifyingSetIterator& other) const
        {
            return &source != &other.source || stack.Top( ) != other.stack.Top( );
        }

    private:
        const SimplifyingSet& source;
        Stack& stack; // After construction, the iterator owns this stack and will need to delete it.
    };

    SimplifyingSetIterator begin( ) const
    {
        if (!root)
        {
            return end( );
        }

        Stack* stack = new Stack( );
        TreeNode* node = root;
        while (true)
        {
            TreeNode* child = node->children[0];
            if (child)
            {
                stack->Push(node);
                node = child;
            }
            else
            {
                stack->Push(node);
                return SimplifyingSetIterator(*this, *stack);
            }
        }
    }

    SimplifyingSetIterator end( ) const
    {
        Stack* stack = new Stack( );
        return SimplifyingSetIterator(*this, *stack);
    }

private:
    TreeNode* root = nullptr;
};
