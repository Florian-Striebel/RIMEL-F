typedef void CalloutFunc_t(void *);
class Callout
{
public:
static void Init();
static void AlarmSignalHandler(int);
static void ExpireCallouts();
Callout();
bool IsActive() const;
bool IsPending() const;
bool Stop();
bool Reset(const timeval &interval, CalloutFunc_t *func, void *arg);
timeval TimeRemaining() const;
private:
static std::list<Callout *> s_activeCallouts;
static bool s_alarmFired;
timeval m_interval;
void *m_arg;
CalloutFunc_t *m_func;
bool m_pending;
};
inline bool
Callout::IsPending() const
{
return (m_pending);
}
inline
Callout::Callout()
: m_arg(0),
m_func(NULL),
m_pending(false)
{
timerclear(&m_interval);
}
