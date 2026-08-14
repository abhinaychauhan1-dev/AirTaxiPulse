/**
 * @file    : src/data/FeatureOverviewCardData.h
 * @brief   : Declares the data object used by feature overview cards.
 * @author  : Abhinay Chauhan (email: abhinay.chauhan1@gmail.com)
 * @version : 1.0.0
 *
 * Copyright (c) 2024
 * Abhinay Chauhan. All rights reserved.
 */

#ifndef FEATUREOVERVIEWCARDDATA_H
#define FEATUREOVERVIEWCARDDATA_H

#include <QObject>

/// @brief Stores immutable title and body content for a feature overview card.
class FeatureOverviewCardData : public QObject
{
    Q_OBJECT

    /// @brief Card title displayed by the UI.
    Q_PROPERTY(QString title READ title CONSTANT)

    /// @brief Descriptive card body displayed by the UI.
    Q_PROPERTY(QString body READ body CONSTANT)

public:
    /// @brief Constructs an immutable feature overview card.
    /// @param title Title text displayed on the card.
    /// @param body Descriptive body text displayed on the card.
    /// @param parent Optional QObject parent for Qt ownership.
    FeatureOverviewCardData(const QString &title,
                            const QString &body,
                            QObject *parent = nullptr);

    /// @brief Returns the card title.
    /// @return Title text displayed by the UI.
    QString title() const;

    /// @brief Returns the card body.
    /// @return Descriptive body text displayed by the UI.
    QString body() const;

private:
    QString m_title;
    QString m_body;
};

#endif // FEATUREOVERVIEWCARDDATA_H