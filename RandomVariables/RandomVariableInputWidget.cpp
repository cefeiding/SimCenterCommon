/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna
// padhye modified

//#include "InputWidgetUQ.h"
#include "RandomVariableInputWidget.h"
#include "ConstantDistribution.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <sectiontitle.h>
#include <QLineEdit>
#include <QTableWidget>

RandomVariableInputWidget::RandomVariableInputWidget(QWidget *parent)
    : SimCenterWidget(parent)
{
    randomVariableClass = QString("Uncertain");

    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeRV();

    //qDebug()<<"\n\n\n   I am here np    \n\n\n   ";

    // padhye adding the button to create table using model view totake in correlation matrix.
    // QPushButton *correlation_button1 = new QPushButton("Add Correlation Matrix");
    // QHBoxLayout *correlation_layout = new QHBoxLayout;
    // correlation_layout->addWidget(correlation_button1);
    // this->setLayout(correlation_layout);

}

RandomVariableInputWidget::RandomVariableInputWidget(QString &theClass, QWidget *parent)
    : SimCenterWidget(parent)
{
    randomVariableClass = theClass;
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeRV(); //makeRV is a private member of RandomVariableInputWidget and is called from here
}

void
RandomVariableInputWidget::setInitialConstantRVs(QStringList &varNamesAndValues)
{
    theRandomVariables.clear();
    delete rvLayout;
    rvLayout = new QVBoxLayout;
    rvLayout->addStretch();
    rv->setLayout(rvLayout);

    int numVar = varNamesAndValues.count();
    for (int i=0; i<numVar; i+= 2) {
        QString varName = varNamesAndValues.at(i);
        QString value = varNamesAndValues.at(i+1);
        double dValue = value.toDouble();
ConstantDistribution *theDistribution = new ConstantDistribution(dValue, 0);
RandomVariable *theRV = new RandomVariable(randomVariableClass, varName, *theDistribution);
connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));


        theRandomVariables.append(theRV);
        rvLayout->insertWidget(rvLayout->count()-1, theRV);
    }
}

void
RandomVariableInputWidget::addConstantRVs(QStringList &varNamesAndValues)
{
    int numVar = varNamesAndValues.count();
    for (int i=0; i<numVar; i+= 2)
    {

    QString varName = varNamesAndValues.at(i);
    QString value = varNamesAndValues.at(i+1);

    double dValue = value.toDouble();
    ConstantDistribution *theDistribution = new ConstantDistribution(dValue, 0);
    RandomVariable *theRV = new RandomVariable(randomVariableClass, varName, *theDistribution);
    connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

    theRandomVariables.append(theRV);
    rvLayout->insertWidget(rvLayout->count()-1, theRV);

    }
}

void
RandomVariableInputWidget::removeRandomVariables(QStringList &varNames)
{
    // find the ones selected & remove them
     int numVar = varNames.count();

    for (int i=0; i<numVar; i++) {
        QString varName = varNames.at(i);

        // find the ones selected & remove them
        int numRandomVariables = theRandomVariables.size();

        for (int j =0; j < numRandomVariables; j++) {
            RandomVariable *theRV = theRandomVariables.at(j);
            if (theRV->variableName->text() == varName) {
                theRV->close();
                rvLayout->removeWidget(theRV);
                theRandomVariables.remove(j);
                theRV->setParent(0);
                delete theRV;
                j=numRandomVariables;
            }
        }
    }
}

RandomVariableInputWidget::~RandomVariableInputWidget()
{

}

// this is called from InputWidgetUQ.cpp
// see
//if (dakotaMethod != 0) {
//    return dakotaMethod->getParameters();
//}

// see the RandomVariableInputWidget.h and this a private member function
void
RandomVariableInputWidget::makeRV(void)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Input Random Variables"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);
    QSpacerItem *spacer3 = new QSpacerItem(50,10);
    QSpacerItem *spacer4 = new QSpacerItem(20,10);



    QPushButton *addRV = new QPushButton();
    addRV->setMinimumWidth(75);
    addRV->setMaximumWidth(75);
    addRV->setText(tr("Add"));
    connect(addRV,SIGNAL(clicked()),this,SLOT(addRandomVariable()));


    QPushButton *removeRV = new QPushButton();
    removeRV->setMinimumWidth(75);
    removeRV->setMaximumWidth(75);
    removeRV->setText(tr("Remove"));
    connect(removeRV,SIGNAL(clicked()),this,SLOT(removeRandomVariable()));



    // padhye, adding the button for correlation matrix, we need to add a condition here
    // that whether the uqMehod selected is that of Dakota and sampling type? only then we need correlation matrix

    QPushButton *addCorrelation = new QPushButton();
    //addCorrelation->setMinimumWidth(250);
    //addCorrelation->setMaximumWidth(280);
    addCorrelation->setText(tr("Add Correlation Matrix"));
    connect(addCorrelation,SIGNAL(clicked()),this,SLOT(addCorrelationMatrix()));
    flag_for_correlationMatrix=0;


    /********************* moving to sampling method input ***************************
    QCheckBox *checkbox =new QCheckBox("Sobolev Index", this);
    connect(checkbox,SIGNAL(clicked(bool)),this,SLOT(addSobolevIndices(bool)));
    flag_for_sobolev_indices=0;
    ******************************************************************************** */

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addRV);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeRV);
    titleLayout->addItem(spacer3);

    {

        titleLayout->addWidget(addCorrelation,0,Qt::AlignTop);
    }

    titleLayout->addItem(spacer4);
    titleLayout->addWidget(checkbox);

    titleLayout->addStretch();

    verticalLayout->addLayout(titleLayout);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    //rv = new QGroupBox(tr(""));
    rv = new QWidget;
  //  rv->setStyleSheet("QGroupBox {background: #E0E0E0}");

    rvLayout = new QVBoxLayout;
    rvLayout->addStretch();
    rv->setLayout(rvLayout);

   // this->addRandomVariable();
     sa->setWidget(rv);
     verticalLayout->addWidget(sa);
     verticalLayout->setSpacing(0);
     verticalLayout->setMargin(0);


}


void RandomVariableInputWidget::variableNameChanged(const QString &newValue)
{

//  qDebug()<<"\n I just changed the name and the new name is       "<<newValue;
  //  qDebug()<<"\n I am exiting the code now         ";
  //  exit(1);

  //updating the variable names in the correlation matrix:

  int numRandomVariables = theRandomVariables.size();

  //qDebug()<<"\n the number of random variables are    "<<numRandomVariables;
  //qDebug()<<"\n the information   ";
  // exit(1);

  if(correlationMatrix!=NULL)
    {

      QStringList table_header;
      for (int i = 0; i < numRandomVariables; i++)
        {
         //     qDebug()<< "\n the variable name is       "<<theRandomVariables.at(i)->getVariableName();
            table_header.append(theRandomVariables.at(i)->getVariableName());
            // RandomVariable *theRV = theRandomVariables.at(i);
            //if (theRV->isSelectedForRemoval())
            //{
                //   theRV->close();
                // rvLayout->removeWidget(theRV);
                // theRandomVariables.remove(i);
                // theRV->setParent(0);
                // delete theRV;
            //}
        }
       // qDebug()<<"\n the table_header is       "<<table_header;
        correlationMatrix->setHorizontalHeaderLabels(table_header);
        correlationMatrix->setVerticalHeaderLabels(table_header);
    }

}


void RandomVariableInputWidget::addRandomVariable(void)
{
   RandomVariable *theRV = new RandomVariable(randomVariableClass);
   theRandomVariables.append(theRV);
   rvLayout->insertWidget(rvLayout->count()-1, theRV);
   connect(this,SLOT(randomVariableErrorMessage(QString)), theRV, SIGNAL(sendErrorMessage(QString)));

   connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

   //if(uq["uqType"].toString()=="sampling")
   {

   if(correlationMatrix!=NULL)
   {
     while (correlationMatrix->rowCount() > 0)
        {
             correlationMatrix->removeRow(0);
        }
        correlationMatrix->clear();
        correlationMatrix->clearContents();
        delete correlationMatrix;
        correlationMatrix=NULL;
        delete correlationtabletitle;
   }

   }

}

void RandomVariableInputWidget::removeRandomVariable(void)
{
    // find the ones selected & remove them
    int numRandomVariables = theRandomVariables.size();
    for (int i = numRandomVariables-1; i >= 0; i--) {
      RandomVariable *theRV = theRandomVariables.at(i);
      if (theRV->isSelectedForRemoval()) {
          theRV->close();
          rvLayout->removeWidget(theRV);
          theRandomVariables.remove(i);
          theRV->setParent(0);
          delete theRV;
      }     
    }


    //qDebug()<<"\n\n\n ";
    // if any of the variable is removed, we remove the correlation matrix and user has to re-add.
    //correlationMatrix->clear();
    //correlationMatrix->clearContents();

    //if(uq["uqType"].toString()=="sampling")
    {

    if(correlationMatrix!=NULL)
    {
         while (correlationMatrix->rowCount() > 0)
         {
                correlationMatrix->removeRow(0);
         }
                correlationMatrix->clear();
                correlationMatrix->clearContents();
                delete correlationMatrix;
                correlationMatrix=NULL;
                delete correlationtabletitle;
    }
    }
}



/*
void RandomVariableInputWidget::addSobolevIndices(bool value)
{

    if(value)
    {

    flag_for_sobolev_indices=1;

    }
    return;

}
*/

// correlation matrix function
void RandomVariableInputWidget::addCorrelationMatrix(void)
{

    int numRandomVariables = theRandomVariables.size();

    //qDebug()<<"\n the number of random variables are    "<<numRandomVariables;
    //qDebug()<<"\n the information   ";
    // exit(1);

    if(correlationMatrix==NULL && numRandomVariables>0)
    {
        flag_for_correlationMatrix=1;

        //QHBoxLayout *correlationtabletableLayout = new QHBoxLayout();

        correlationtabletitle=new SectionTitle();


        correlationtabletitle->setText(tr("Correlation Matrix"));
//        correlationtabletitle->setMinimumWidth(200);
     //   correlationtabletitle->resize();


        verticalLayout->addWidget(correlationtabletitle);

        //QSpacerItem *spacer1 = new QSpacerItem(50,10);
        this->correlationMatrix = new QTableWidget();

        verticalLayout->addWidget(correlationMatrix);



    // find the ones selected & remove them

   // This example shows how we might create a status string for reporting progress while processing a list of files:

   // QString i;           // current file's number
   // QString total;       // number of files to process
   // QString fileName;    // current file's name

   // QString status = QString("Processing file %1 of %2: %3")
   //                 .arg(i).arg(total).arg(fileName);

   // First, arg(i) replaces %1. Then arg(total) replaces %2. Finally, arg(fileName) replaces %3.

    //correlationMatrix->setWindowTitle("Correlation Matrix");
    correlationMatrix->setRowCount(numRandomVariables);
    correlationMatrix->setColumnCount(numRandomVariables);

   // correlationMatrix->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  //  correlationMatrix->setMaximumWidth(numRandomVariables*55);
  //  correlationMatrix->setMinimumWidth(10);



    //correlationMatrix->setMaximumHeight(numRandomVariables*50);
    //correlationMatrix->setMinimumHeight(10);

    //newItem = new QTableWidgetItem(tr("%......1").arg((numRandomVariables+1)*(numRandomVariables+1)));
    //correlationMatrix->setItem(0,0, newItem);
    //correlationMatrix->setItem(0, 0, newItem);


    // if the number of variables is zero then show a warning.
  //   qDebug()<<"\n \n  \n  \n";
  //   qDebug()<<"\n values of theRandomVariables is   \n";
  //   qDebug()<<"\n   \n"<<theRandomVariables.at(0)->getVariableName();
  //  qDebug()<<"\n \n    \n  The value of numRandomVariables is  \n \n \n  "<<numRandomVariables;


    QStringList table_header;

    for (int i = 0; i < numRandomVariables; i++)
    {

     //     qDebug()<< "\n the variable name is       "<<theRandomVariables.at(i)->getVariableName();

        table_header.append(theRandomVariables.at(i)->getVariableName());

        // RandomVariable *theRV = theRandomVariables.at(i);
        //if (theRV->isSelectedForRemoval())
        //{
            //   theRV->close();
            // rvLayout->removeWidget(theRV);
            // theRandomVariables.remove(i);
            // theRV->setParent(0);
            // delete theRV;
        //}
    }

   // qDebug()<<"\n the table_header is       "<<table_header;
    correlationMatrix->setHorizontalHeaderLabels(table_header);
    correlationMatrix->setVerticalHeaderLabels(table_header);

    correlationMatrix->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    correlationMatrix->setSizePolicy(QSizePolicy::Policy::Minimum,QSizePolicy::Policy::Minimum);

    for(int i = 0; i < numRandomVariables; i++)
    {
        correlationMatrix->setColumnWidth(i,100 );

        for(int j = 0; j < numRandomVariables; j++)
        {



            QTableWidgetItem *newItem;

            if(i==j)
            {
            newItem = new QTableWidgetItem("1.0");
            }else
                 {
                    newItem = new QTableWidgetItem("0.0");
                 }
            correlationMatrix->setItem(i,j, newItem);

        }
    }

    correlationMatrix->resizeColumnsToContents();
    correlationMatrix->resizeRowsToContents();

    //correlationMatrix->horizontalHeader()->setStretchLastSection(True)
    //correlationMatrix->verticalHeader()->setStretchLastSection(True)

    }
 //   exit(1);
}

// loop over random variables, removing from layout & deleting

void
RandomVariableInputWidget::clear(void)
{
  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theRandomVariables.size(); ++i) {
    RandomVariable *theRV = theRandomVariables.at(i);
    rvLayout->removeWidget(theRV);
    delete theRV;
  }
  theRandomVariables.clear();


}


// this will fill in the information of random variables into the json


bool
RandomVariableInputWidget::outputToJSON(QJsonObject &rvObject)
{
    bool result = true;
    QJsonArray rvArray;
    for (int i = 0; i <theRandomVariables.size(); ++i) {
        QJsonObject rv;
        if (theRandomVariables.at(i)->outputToJSON(rv)) {
            rvArray.append(rv);

        } else {
            qDebug() << "OUTPUT FAILED" << theRandomVariables.at(i)->variableName->text();
            result = false;
        }
    }

    rvObject["randomVariables"]=rvArray;

    // added by padhye

    QJsonArray testingjsonarray;

    int correlation_flad_check =1;
    for (int i = 0; i <theRandomVariables.size(); ++i)
      {
          for (int j = 0; j <theRandomVariables.size(); ++j)
          {
              // this gets the pointer to an item of the table at index i,j


          //    qDebug()<<"\n I am just here before cellItemFromTable \n";

            //  qDebug()<<"\n I am just here before cellItemFromTable \n";

             // qDebug()<<"\n I am just here before cellItemFromTable \n";

              QTableWidgetItem *cellItemFromTable=NULL;

              if(correlationMatrix!=NULL){cellItemFromTable= correlationMatrix->item(i,j);}
              else {correlation_flad_check=0;}

          //    qDebug()<<"\n checking if cellItemFromTabele defined      "<<cellItemFromTable;


              if(cellItemFromTable!=NULL)
              {
                    qDebug()<<"\n the value of   item is      "<<i<<"     "<<j<<((cellItemFromTable->text()).toDouble())<<"\n";

                    double value=((cellItemFromTable->text()).toDouble());
                    testingjsonarray.append(value);
              }else {correlation_flad_check=0;}
            }
    }

    if(correlation_flad_check==1)
    {rvObject["uncertain_correlation_matrix"]=testingjsonarray;}

    /*
    if(flag_for_sobolev_indices==1)
    {
        rvObject["sobelov_indices"]=1;

    }
    */

    return result;



}


QStringList
RandomVariableInputWidget::getRandomVariableNames(void)
{
    QStringList results;
    for (int i = 0; i <theRandomVariables.size(); ++i) {
        results.append(theRandomVariables.at(i)->getVariableName());
    }
    return results;
}

bool
RandomVariableInputWidget::inputFromJSON(QJsonObject &rvObject)
{
  bool result = true;

  // clean out current list
  this->clear();

  //
  // go get randomvariables array from the JSON object
  // for each object in array:
  //    1)get it'is type,
  //    2)instantiate one
  //    4) get it to input itself
  //    5) finally add it to layout
  //


  // get array
  if (rvObject.contains("randomVariables"))
      if (rvObject["randomVariables"].isArray()) {

          QJsonArray rvArray = rvObject["randomVariables"].toArray();

          // foreach object in array
          foreach (const QJsonValue &rvValue, rvArray) {

              QJsonObject rvObject = rvValue.toObject();

              if (rvObject.contains("variableClass")) {
                QJsonValue typeRV = rvObject["variableClass"];
                RandomVariable *theRV = 0;
                QString classType = typeRV.toString();
                theRV = new RandomVariable(classType);
      connect(theRV->variableName, SIGNAL(textEdited(const QString &)), this, SLOT(variableNameChanged(const QString &)));

                connect(theRV,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));

                if (theRV->inputFromJSON(rvObject)) { // this method is where type is set
                    theRandomVariables.append(theRV);
                    rvLayout->insertWidget(rvLayout->count()-1, theRV);
                } else {
                    result = false;
                }
              } else {
                  result = false;
              }
          }

        //  qDebug()<<"\n\n\n   I am here just before adding correlation from default loading json    \n\n\n   ";

          // adding/inializing correlation matrix if the data is loaded from json
          {
            addCorrelationMatrix();

          }
      }
  return result;
}


void
RandomVariableInputWidget::errorMessage(QString message){
    emit sendErrorMessage(message);
}

