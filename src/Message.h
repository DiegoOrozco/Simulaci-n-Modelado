#ifndef MESSAGE_H
#define MESSAGE_H
class Message
{
    private:
        // estado en el que se encuentra el mensaje
        bool message_send;
        // si el mensaje es enviado por error
        bool error;     
        // tiempo que ocurre el timeout del mensaje
        double timeout;
        // identificador del mensaje
        int id;
        // tiempo en que llega a A
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