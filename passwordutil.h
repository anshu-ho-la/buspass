#ifndef PASSWORDUTIL_H
#define PASSWORDUTIL_H

#include <QString>
#include <QCryptographicHash>

// Shared password hashing helper, used anywhere a password is stored or
// checked (signup, login, profile password changes, admin account creation).
// Uses SHA-256 - not reversible, so the database never stores a usable
// plain-text password.
namespace PasswordUtil {

inline QString hash(const QString &plainPassword)
{
    return QString(QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// A SHA-256 hex digest is always exactly 64 lowercase hex characters, which
// virtually no real plain-text password will ever accidentally match - used
// by the one-time migration in Database::connect() to detect and upgrade
// any password rows that predate hashing being introduced.
inline bool looksHashed(const QString &value)
{
    if (value.length() != 64) {
        return false;
    }
    for (const QChar &c : value) {
        if (!c.isDigit() && !(c >= 'a' && c <= 'f')) {
            return false;
        }
    }
    return true;
}

} // namespace PasswordUtil

#endif // PASSWORDUTIL_H
