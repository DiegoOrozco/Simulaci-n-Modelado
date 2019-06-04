#ifndef MESSAGE_H
#define MESSAGE_H
class Message
{
    private:
        bool message_arrived = false;
        bool error = false;		
        unsigned int timeout = 0;

    public:
        Message();
        ~Message();

        bool get_arrived();
        void set_arrived(bool value);

        bool get_error();
        void set_error(bool value);

        bool get_timeout();
        void set_timeout(unsigned int value);
};

#endif // MESSAGE_H