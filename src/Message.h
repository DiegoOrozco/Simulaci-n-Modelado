#ifndef MESSAGE_H
#define MESSAGE_H
class Message
{
    private:
        bool message_send;
        bool error;     
        double timeout;
        int id;
        double time_arrival;

    public:
        Message();
        Message(bool send_message, int new_id);
        Message(bool send_message, int new_id, bool error);
        ~Message();

        inline bool get_send() { return this->message_send; }
        void set_send(bool value);

        inline bool get_error() { return this->error; }
        void set_error(bool value);

        inline double get_timeout() { return this->timeout; }
        void set_timeout(double value);

        inline int get_id() { return this->id; }
        void set_id(int value);        

        inline double get_time() { return this->time_arrival; }
        void set_time(int value);        

};

#endif // MESSAGE_H