#ifndef SESSION_H
#define SESSION_H

class Session
{

private:
    Session() {
        m_id = -1;
    }

    int m_id;

public:
    static Session& instance() {
        static Session s;
        return s;
    }

    void login(int id) {
        m_id = id;
    }

    int id(){
        return m_id;
    }
};

#endif // SESSION_H