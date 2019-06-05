#ifndef MESSAGE_H
#define MESSAGE_H
class Message
{
    private:
        bool message_send;
        bool error;     
        unsigned int timeout;
        int id;

    public:
        Message();
        Message(bool send_message, int new_id);
        ~Message();

        inline bool get_send() { return this->message_send; }
        void set_send(bool value);

        inline bool get_error() { return this->error; }
        void set_error(bool value);

        inline bool get_timeout() { return this->timeout; }
        void set_timeout(unsigned int value);

        inline bool get_id() { return this->id; }
        void set_id(int value);        
};

#endif // MESSAGE_H