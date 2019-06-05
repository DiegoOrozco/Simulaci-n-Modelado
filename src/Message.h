#ifndef MESSAGE_H
#define MESSAGE_H
class Message
{
    private:
        bool message_arrived;
        bool error;     
        unsigned int timeout;

    public:
        Message();
        ~Message();

        inline bool get_arrived() { return this->message_arrived; }
        void set_arrived(bool value);

        inline bool get_error() { return this->error; }
        void set_error(bool value);

        inline bool get_timeout() { return this->timeout; }
        void set_timeout(unsigned int value);
};

#endif // MESSAGE_H