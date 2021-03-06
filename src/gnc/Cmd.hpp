/*
 * Cmd.hpp
 * Copyright (C) 2010 Christian Stimming
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, contact:
 *
 * Free Software Foundation           Voice:  +1-617-542-5942
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652
 * Boston, MA  02110-1301,  USA       gnu@gnu.org
 */

#ifndef GNC_CMD_HPP
#define GNC_CMD_HPP

#include <QUndoCommand>
#include <QDate>
#include <gnc/WeakPointer.hpp>
#include <gncmm/Numeric.hpp>

namespace gnc
{

class Account;
class Split;
class TmpSplit;
class TmpTransaction;
class Transaction;

/** This is a templated implementation of a QUndoCommand class. This
 * implements the Command pattern: Any instance of this class
 * represents the change of one member variable (of type ValueT) in an
 * instance of a TargetT object.
 *
 * The fun part is that we can pass pointers to the getter and setter
 * functions into this class' constructor, and the redo()/undo()
 * implementations follow naturally. Hence, this class is already
 * sufficient for any Command that is executed by a particular Setter
 * function.
 *
 * As we need to keep a reference to the original object, it is
 * relevant who owns the life cycle of that original object. For
 * simplicity, we constrain this current implementation only to
 * classes which are implementations of a WeakPointer<...> which by
 * definition keeps a C pointer to the original object, and we hope
 * that one lives longer than we do.
 */
template<class TargetT, class ValueT, typename SetterFunc = void (TargetT::*)(const ValueT&)>
class Cmd : public QUndoCommand
{
public:
    /// The base class
    typedef QUndoCommand base_class;

    /// Type of the target object on which this command is applied
    typedef TargetT target_type;

    /// Type of the value that is set by this command
    typedef ValueT value_type;

    /// Type of the setter function to set the value in the target object
    typedef SetterFunc setter_func;

    /// Type of the getter function to retrieve the current value from the target object
    typedef value_type (target_type::*getter_func)() const;

    /** Constructor with the to-be-manipulated object "targetPtr", the setter
     * function, the previous value, and the new value.
     *
     * @param text The QUndoCommand's text which will be displayed in the Undo action.
     * @param targetPtr Reference to the target object on which this command is applied.
     * @param setter Pointer to function which sets the value in the target object
     * @param previousValue The previous value, in case this command needs to be undone
     * @param newValue The new value to be set
     * @param parent The parent QUndoCommand instance, or NULL.
     */
    Cmd(const QString& text,
        Glib::RefPtr<target_type> targetPtr,
        setter_func setter,
        const value_type& previousValue,
        const value_type& newValue,
        QUndoCommand *parent = 0)
            : base_class(text, parent)
            , m_target(targetPtr)
            , m_setter(setter)
            , m_previousValue(previousValue)
            , m_newValue(newValue)
    {
        Q_ASSERT(m_target);
        Q_ASSERT(m_setter);
    }

    virtual void redo()
    {
        set(m_newValue);
    }

    virtual void undo()
    {
        set(m_previousValue);
    }


private:
    void set(const value_type& value)
    {
        // Uh oh.  The calling syntax for pointer-to-member
        // variables (here: m_setter) looks rather weird:
        (m_target.operator->()->*m_setter)(value);
    }

protected:
    Glib::RefPtr<target_type> m_target;
    setter_func m_setter;
    value_type m_previousValue;
    value_type m_newValue;

};

namespace cmd
{

// This is the collection of command objects which are already
// provided for the different data types and their simple
// members. Just create one of those, add it to a QUndoStack, and
// magically the values will change with undo/redo back and
// forth. Spooky, IMHO.
// QUndoCommand* setSplitMemo(Split& split, const QString& newValue);
// QUndoCommand* setSplitAction(Split& t, const QString& newValue);
QUndoCommand* setSplitAccount(Glib::RefPtr<Split> t, Glib::RefPtr<Account> newValue);
QUndoCommand* setSplitAccount(TmpSplit& t, Glib::RefPtr<Account> newValue);
QUndoCommand* setSplitReconcile(Glib::RefPtr<Split> t, char newValue);
QUndoCommand* setSplitReconcile(TmpSplit& t, char newValue);
// QUndoCommand* setSplitAmount(Glib::RefPtr<Split> t, const Numeric& newValue);
// QUndoCommand* setSplitValue(Glib::RefPtr<Split> t, const Numeric& newValue);
QUndoCommand* setTransactionNum(Glib::RefPtr<Transaction> t, const QString& newValue);
QUndoCommand* setTransactionNum(TmpTransaction& t, const QString& newValue);
QUndoCommand* setTransactionDescription(Glib::RefPtr<Transaction> t, const QString& newValue);
QUndoCommand* setTransactionDescription(TmpTransaction& t, const QString& newValue);
// QUndoCommand* setTransactionNotes(Glib::RefPtr<Transaction> t, const QString& newValue);
QUndoCommand* setTransactionDate(Glib::RefPtr<Transaction> t, const QDate& newValue);
QUndoCommand* setTransactionDate(TmpTransaction& t, const QDate& newValue);
QUndoCommand* setSplitValueAndAmount(Glib::RefPtr<Split> t, const Numeric& newValue);
QUndoCommand* setSplitValueAndAmount(TmpSplit& t, const Numeric& newValue);
QUndoCommand* destroyTransaction(Glib::RefPtr<Transaction> t);
QUndoCommand* commitNewTransaction(const TmpTransaction& t);

} // END namespace cmd

} // END namespace gnc

#endif
