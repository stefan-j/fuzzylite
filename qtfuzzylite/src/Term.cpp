/*
 * Term.cpp
 *
 *  Created on: 11/12/2012
 *      Author: jcrada
 */

#include "fl/qt/Term.h"

#include "fl/qt/Viewer.h"
#include "fl/qt/Window.h"
#include "fl/qt/Wizard.h"
#include "fl/qt/Model.h"

#include <fl/Headers.h>

#include <QtGui/QMessageBox>

namespace fl {
    namespace qt {

        Term::Term(QWidget* parent, Qt::WindowFlags f)
        : QDialog(parent, f),
        ui(new Ui::Term), viewer(NULL), dummyVariable(new Variable),
        indexOfEditingTerm(-1) {
            setWindowFlags(Qt::Tool);
        }

        Term::~Term() {
            disconnect();
            delete ui;
            for (std::size_t i = 0; i < _basicTerms.size(); ++i) {
                delete _basicTerms[i];
            }
            for (std::size_t i = 0; i < _extendedTerms.size(); ++i) {
                delete _extendedTerms[i];
            }
            for (std::size_t i = 0; i < _edgeTerms.size(); ++i) {
                delete _edgeTerms[i];
            }
            for (int i = dummyVariable->numberOfTerms() - 1; i >= 0; --i) {
                delete dummyVariable->removeTerm(i);
            }
            delete dummyVariable;

        }

        void Term::loadTerms(scalar min, scalar max) {
            scalar average = (min + max) / 2;
            scalar diff = (max - min);
            //It MUST follow the order of the toolbox.
            _basicTerms.push_back(new Triangle("", min, average, max));
            _basicTerms.push_back(new Trapezoid("", min, min + .25 * diff,
                    min + .75 * diff, max));
            _basicTerms.push_back(new Rectangle("", min + .25 * diff, min + .75 * max));

            std::vector<std::pair<scalar, scalar> > xy;
            xy.push_back(std::make_pair(min, 0));
            xy.push_back(std::make_pair(min + .25 * diff, 1));
            xy.push_back(std::make_pair(min + .5 * diff, .5));
            xy.push_back(std::make_pair(min + .75 * diff, 1));
            xy.push_back(std::make_pair(max, 0));
            _basicTerms.push_back(new Discrete("", xy));

            _extendedTerms.push_back(new Gaussian("", average, .2 * diff));
            _extendedTerms.push_back(new GaussianProduct("", average, .2 * diff, average, .2 * diff));
            _extendedTerms.push_back(new Bell("", average, .25 * diff, 3.0));
            _extendedTerms.push_back(new PiShape("", min, average, average, max));
            _extendedTerms.push_back(new SigmoidDifference("", min + .25 * diff, 20 / diff,
                    min + .75 * diff, 20 / diff));
            _extendedTerms.push_back(new SigmoidProduct("", min + .25 * diff, 20 / diff,
                    min + .75 * diff, -20 / diff));

            _edgeTerms.push_back(new Ramp("", min, max));
            _edgeTerms.push_back(new Sigmoid("", average, 20 / diff));
            _edgeTerms.push_back(new SShape("", min, max));
            _edgeTerms.push_back(new ZShape("", min, max));

        }

        void Term::setup(const fl::Variable& variable, const fl::Term* edit) {
            loadTerms(variable.getMinimum(), variable.getMaximum());

            dummyVariable->setMinimum(variable.getMinimum());
            dummyVariable->setMaximum(variable.getMaximum());
            for (int i = 0; i < variable.numberOfTerms(); ++i) {
                dummyVariable->addTerm(variable.getTerm(i)->copy());
                if (variable.getTerm(i) == edit) {
                    indexOfEditingTerm = i;
                }
            }

            ui->setupUi(this);

            viewer = new Viewer;
            viewer->setup(dummyVariable);
            ui->lyt_terms->insertWidget(0, viewer);

            setWindowTitle(edit ? "Edit term" : "Add term");

            layout()->setSizeConstraint(QLayout::SetFixedSize);
            this->adjustSize();

            QRect scr = Window::mainWindow()->geometry();
            move(scr.center() - rect().center());

            _sbx.clear();
            _sbx.push_back(ui->sbx_bell_center);
            _sbx.push_back(ui->sbx_bell_width);
            _sbx.push_back(ui->sbx_bell_slope);

            _sbx.push_back(ui->sbx_gaussian_center);
            _sbx.push_back(ui->sbx_gaussian_width);

            _sbx.push_back(ui->sbx_gaussian_prod_center_a);
            _sbx.push_back(ui->sbx_gaussian_prod_center_b);
            _sbx.push_back(ui->sbx_gaussian_prod_width_a);
            _sbx.push_back(ui->sbx_gaussian_prod_width_b);

            _sbx.push_back(ui->sbx_pishape_a);
            _sbx.push_back(ui->sbx_pishape_b);
            _sbx.push_back(ui->sbx_pishape_c);
            _sbx.push_back(ui->sbx_pishape_d);

            _sbx.push_back(ui->sbx_ramp_start);
            _sbx.push_back(ui->sbx_ramp_end);

            _sbx.push_back(ui->sbx_rectangle_max);
            _sbx.push_back(ui->sbx_rectangle_min);

            _sbx.push_back(ui->sbx_sshape_start);
            _sbx.push_back(ui->sbx_sshape_end);

            _sbx.push_back(ui->sbx_sigmoid_inflection);
            _sbx.push_back(ui->sbx_sigmoid_slope);

            _sbx.push_back(ui->sbx_sigmoid_diff_falling);
            _sbx.push_back(ui->sbx_sigmoid_diff_left);
            _sbx.push_back(ui->sbx_sigmoid_diff_rising);
            _sbx.push_back(ui->sbx_sigmoid_diff_right);

            _sbx.push_back(ui->sbx_sigmoid_prod_falling);
            _sbx.push_back(ui->sbx_sigmoid_prod_left);
            _sbx.push_back(ui->sbx_sigmoid_prod_rising);
            _sbx.push_back(ui->sbx_sigmoid_prod_right);

            _sbx.push_back(ui->sbx_trapezoid_a);
            _sbx.push_back(ui->sbx_trapezoid_b);
            _sbx.push_back(ui->sbx_trapezoid_c);
            _sbx.push_back(ui->sbx_trapezoid_d);

            _sbx.push_back(ui->sbx_triangle_a);
            _sbx.push_back(ui->sbx_triangle_b);
            _sbx.push_back(ui->sbx_triangle_c);

            _sbx.push_back(ui->sbx_zshape_start);
            _sbx.push_back(ui->sbx_zshape_end);

            for (std::size_t i = 0; i < _sbx.size(); ++i) {
                _sbx[i]->setMinimum(-1000);
                _sbx[i]->setMaximum(1000);
                _sbx[i]->setValue(0.0);
                _sbx[i]->setSingleStep(
                        (dummyVariable->getMaximum() - dummyVariable->getMinimum()) / 100);
            }

            for (std::size_t i = 0; i < _basicTerms.size(); ++i) {
                loadFrom(_basicTerms[i]);
            }
            for (std::size_t i = 0; i < _extendedTerms.size(); ++i) {
                loadFrom(_extendedTerms[i]);
            }
            for (std::size_t i = 0; i < _edgeTerms.size(); ++i) {
                loadFrom(_edgeTerms[i]);
            }

            ui->basicTermToolbox->setCurrentIndex(0);
            ui->extendedTermToolbox->setCurrentIndex(0);
            ui->edgeTermToolbox->setCurrentIndex(0);

            connect();


            if (edit) {
                loadFrom(edit);
                ui->led_name->setText(QString::fromStdString(edit->getName()));
            } else {
                dummyVariable->addTerm(_basicTerms[0]->copy()); //Add the triangle by default
                indexOfEditingTerm = dummyVariable->numberOfTerms() - 1;
                setCurrentToolbox(0);
            }

        }

        void Term::accept() {
            dummyVariable->getTerm(indexOfEditingTerm)->setName(
                    ui->led_name->text().toStdString());
            QDialog::accept();
        }

        void Term::connect() {
            QObject::connect(ui->basicTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)), Qt::QueuedConnection);
            QObject::connect(ui->extendedTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)), Qt::QueuedConnection);
            QObject::connect(ui->edgeTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)), Qt::QueuedConnection);

            QObject::connect(ui->tabTerms, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeTab(int)), Qt::QueuedConnection);

            QObject::connect(viewer, SIGNAL(valueChanged(double)),
                    this, SLOT(showSelectedTerm()));

            QObject::connect(ui->btn_wizard, SIGNAL(clicked()),
                    this, SLOT(onClickWizard()));

            //BASIC
            //Triangle
            QObject::connect(ui->sbx_triangle_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));
            QObject::connect(ui->sbx_triangle_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));
            QObject::connect(ui->sbx_triangle_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));

            //Trapezoid
            QObject::connect(ui->sbx_trapezoid_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::connect(ui->sbx_trapezoid_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::connect(ui->sbx_trapezoid_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::connect(ui->sbx_trapezoid_d, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));

            //Rectangle
            QObject::connect(ui->sbx_rectangle_min, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRectangle(double)));
            QObject::connect(ui->sbx_rectangle_max, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRectangle(double)));

            //Discrete
            QObject::connect(ui->btn_discrete_parse, SIGNAL(clicked()),
                    this, SLOT(onClickDiscreteParser()));

            //EXTENDED
            //Gaussian
            QObject::connect(ui->sbx_gaussian_center, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussian(double)));
            QObject::connect(ui->sbx_gaussian_width, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussian(double)));

            //GaussianProduct
            QObject::connect(ui->sbx_gaussian_prod_center_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::connect(ui->sbx_gaussian_prod_center_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::connect(ui->sbx_gaussian_prod_width_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::connect(ui->sbx_gaussian_prod_width_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            //Bell
            QObject::connect(ui->sbx_bell_center, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));
            QObject::connect(ui->sbx_bell_width, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));
            QObject::connect(ui->sbx_bell_slope, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));

            //Pi-Shape
            QObject::connect(ui->sbx_pishape_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::connect(ui->sbx_pishape_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::connect(ui->sbx_pishape_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::connect(ui->sbx_pishape_d, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));

            //SigmoidDiff
            QObject::connect(ui->sbx_sigmoid_diff_rising, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::connect(ui->sbx_sigmoid_diff_left, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::connect(ui->sbx_sigmoid_diff_falling, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::connect(ui->sbx_sigmoid_diff_right, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));

            //SigmoidProd
            QObject::connect(ui->sbx_sigmoid_prod_rising, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::connect(ui->sbx_sigmoid_prod_left, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::connect(ui->sbx_sigmoid_prod_falling, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::connect(ui->sbx_sigmoid_prod_right, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            //EDGES
            //Ramp
            QObject::connect(ui->sbx_ramp_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRamp(double)));
            QObject::connect(ui->sbx_ramp_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRamp(double)));

            //Sigmoid
            QObject::connect(ui->sbx_sigmoid_inflection, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoid(double)));
            QObject::connect(ui->sbx_sigmoid_slope, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoid(double)));

            //S-Shape
            QObject::connect(ui->sbx_sshape_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSShape(double)));
            QObject::connect(ui->sbx_sshape_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSShape(double)));

            //Z-Shape
            QObject::connect(ui->sbx_zshape_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxZShape(double)));
            QObject::connect(ui->sbx_zshape_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxZShape(double)));
        }

        void Term::disconnect() {
            QObject::disconnect(ui->basicTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)));
            QObject::disconnect(ui->extendedTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)));
            QObject::disconnect(ui->edgeTermToolbox, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeToolBoxIndex(int)));

            QObject::disconnect(ui->tabTerms, SIGNAL(currentChanged(int)),
                    this, SLOT(onChangeTab(int)));
            QObject::disconnect(viewer, SIGNAL(valueChanged(double)),
                    this, SLOT(redraw()));

            //BASIC
            //Triangle
            QObject::disconnect(ui->sbx_triangle_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));
            QObject::disconnect(ui->sbx_triangle_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));
            QObject::disconnect(ui->sbx_triangle_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTriangle(double)));

            //Trapezoid
            QObject::disconnect(ui->sbx_trapezoid_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::disconnect(ui->sbx_trapezoid_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::disconnect(ui->sbx_trapezoid_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));
            QObject::disconnect(ui->sbx_trapezoid_d, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxTrapezoid(double)));

            //Rectangle
            QObject::disconnect(ui->sbx_rectangle_min, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRectangle(double)));
            QObject::disconnect(ui->sbx_rectangle_max, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRectangle(double)));

            //Discrete
            QObject::disconnect(ui->btn_discrete_parse, SIGNAL(clicked()),
                    this, SLOT(onClickDiscreteParser()));

            //EXTENDED
            //Gaussian
            QObject::disconnect(ui->sbx_gaussian_center, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussian(double)));
            QObject::disconnect(ui->sbx_gaussian_width, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussian(double)));

            //GaussianProduct
            QObject::disconnect(ui->sbx_gaussian_prod_center_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::disconnect(ui->sbx_gaussian_prod_center_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::disconnect(ui->sbx_gaussian_prod_width_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            QObject::disconnect(ui->sbx_gaussian_prod_width_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxGaussianProduct(double)));
            //Bell
            QObject::disconnect(ui->sbx_bell_center, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));
            QObject::disconnect(ui->sbx_bell_width, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));
            QObject::disconnect(ui->sbx_bell_slope, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxBell(double)));

            //Pi-Shape
            QObject::disconnect(ui->sbx_pishape_a, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::disconnect(ui->sbx_pishape_b, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::disconnect(ui->sbx_pishape_c, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));
            QObject::disconnect(ui->sbx_pishape_d, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxPiShape(double)));

            //SigmoidDiff
            QObject::disconnect(ui->sbx_sigmoid_diff_rising, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::disconnect(ui->sbx_sigmoid_diff_left, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::disconnect(ui->sbx_sigmoid_diff_falling, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));
            QObject::disconnect(ui->sbx_sigmoid_diff_right, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidDiff(double)));

            //SigmoidProd
            QObject::disconnect(ui->sbx_sigmoid_prod_rising, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::disconnect(ui->sbx_sigmoid_prod_left, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::disconnect(ui->sbx_sigmoid_prod_falling, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            QObject::disconnect(ui->sbx_sigmoid_prod_right, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoidProd(double)));
            //EDGES
            //Ramp
            QObject::disconnect(ui->sbx_ramp_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRamp(double)));
            QObject::disconnect(ui->sbx_ramp_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxRamp(double)));

            //Sigmoid
            QObject::disconnect(ui->sbx_sigmoid_inflection, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoid(double)));
            QObject::disconnect(ui->sbx_sigmoid_slope, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSigmoid(double)));

            //S-Shape
            QObject::disconnect(ui->sbx_sshape_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSShape(double)));
            QObject::disconnect(ui->sbx_sshape_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxSShape(double)));

            //Z-Shape
            QObject::disconnect(ui->sbx_zshape_start, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxZShape(double)));
            QObject::disconnect(ui->sbx_zshape_end, SIGNAL(valueChanged(double)),
                    this, SLOT(onChangeSpinBoxZShape(double)));


        }

        void Term::showEvent(QShowEvent* event) {
            redraw();
            QWidget::showEvent(event);
        }

        void Term::resizeEvent(QResizeEvent* event) {
            redraw();
            QWidget::resizeEvent(event);
        }

        void Term::redraw() {
            viewer->refresh();
            showSelectedTerm();
        }

        void Term::showSelectedTerm() {
            viewer->draw(dummyVariable->getTerm(indexOfEditingTerm));
        }

        fl::Term* Term::selectedTerm() const {
            return dummyVariable->getTerm(indexOfEditingTerm);
        }

        void Term::setCurrentToolbox(int index) {
            ui->basicTermToolbox->setVisible(index == 0);
            ui->extendedTermToolbox->setVisible(index == 1);
            ui->edgeTermToolbox->setVisible(index == 2);
            ui->tabTerms->setCurrentIndex(index);
        }

        /**
         * Events
         */

        void Term::onClickWizard() {
            Wizard window(this);
            window.setup(ui->led_name->text().toStdString());
            window.ui->sbx_separation->setMinimum(-1000);
            window.ui->sbx_separation->setMaximum(1000);
            window.ui->sbx_separation->setSingleStep(
                    (dummyVariable->getMaximum() - dummyVariable->getMinimum()) / 100);

            if (not window.exec()) return;

            int copies = window.ui->sbx_copies->value();
            QStringList names = window.ui->ptx_terms->toPlainText().split(
                    QRegExp("\\s+"));
            while (names.size() < copies) {
                names.append("");
            }

            double distance = window.ui->sbx_separation->value();
            for (int i = 0; i < copies; ++i) {
                fl::Term* copy = dummyVariable->getTerm(indexOfEditingTerm)->copy();
                copy->setName(names[i].toStdString());
                scalar separationDistance = distance * (i + 1);
                dummyVariable->insertTerm(copy, indexOfEditingTerm + i + 1);
                //BASIC
                if (copy->className() == Triangle().className()) {
                    Triangle* term = dynamic_cast<Triangle*> (copy);
                    term->setA(term->getA() + separationDistance);
                    term->setB(term->getB() + separationDistance);
                    term->setC(term->getC() + separationDistance);

                } else if (copy->className() == Trapezoid().className()) {
                    Trapezoid* term = dynamic_cast<Trapezoid*> (copy);
                    term->setA(term->getA() + separationDistance);
                    term->setB(term->getB() + separationDistance);
                    term->setC(term->getC() + separationDistance);
                    term->setD(term->getD() + separationDistance);

                } else if (copy->className() == Rectangle().className()) {
                    Rectangle* term = dynamic_cast<Rectangle*> (copy);
                    term->setMinimum(term->getMinimum() + separationDistance);
                    term->setMaximum(term->getMaximum() + separationDistance);

                } else if (copy->className() == Discrete().className()) {
                    Discrete* term = dynamic_cast<Discrete*> (copy);
                    for (std::size_t i = 0; i < term->x.size(); ++i) {
                        term->x[i] = term->x[i] + separationDistance;
                    }

                    //EXTENDED
                } else if (copy->className() == Gaussian().className()) {
                    Gaussian* term = dynamic_cast<Gaussian*> (copy);
                    term->setMean(term->getMean() + separationDistance);

                } else if (copy->className() == GaussianProduct().className()) {
                    GaussianProduct* term = dynamic_cast<GaussianProduct*> (copy);
                    term->setMeanA(term->getMeanA() + separationDistance);
                    term->setMeanB(term->getMeanB() + separationDistance);

                } else if (copy->className() == Bell().className()) {
                    Bell* term = dynamic_cast<Bell*> (copy);
                    term->setCenter(term->getCenter() + separationDistance);

                } else if (copy->className() == PiShape().className()) {
                    PiShape* term = dynamic_cast<PiShape*> (copy);
                    term->setB(term->getB() + separationDistance);
                    term->setC(term->getC() + separationDistance);

                } else if (copy->className() == SigmoidDifference().className()) {
                    SigmoidDifference* term = dynamic_cast<SigmoidDifference*> (copy);
                    term->setLeftInflection(term->getLeftInflection() + separationDistance);
                    term->setRightInflection(term->getRightInflection() + separationDistance);

                } else if (copy->className() == SigmoidProduct().className()) {
                    SigmoidProduct* term = dynamic_cast<SigmoidProduct*> (copy);
                    term->setLeftInflection(term->getLeftInflection() + separationDistance);
                    term->setRightInflection(term->getRightInflection() + separationDistance);

                    //EDGE
                } else if (copy->className() == Ramp().className()) {
                    Ramp* term = dynamic_cast<Ramp*> (copy);
                    term->setStart(term->getStart() + separationDistance);
                    term->setEnd(term->getEnd() + separationDistance);

                } else if (copy->className() == Sigmoid().className()) {
                    Sigmoid* term = dynamic_cast<Sigmoid*> (copy);
                    term->setInflection(term->getInflection() + separationDistance);

                } else if (copy->className() == SShape().className()) {
                    SShape* term = dynamic_cast<SShape*> (copy);
                    term->setStart(term->getStart() + separationDistance);
                    term->setEnd(term->getEnd() + separationDistance);

                } else if (copy->className() == ZShape().className()) {
                    ZShape* term = dynamic_cast<ZShape*> (copy);
                    term->setStart(term->getStart() + separationDistance);
                    term->setEnd(term->getEnd() + separationDistance);
                } else {
                    std::ostringstream ex;
                    ex << "[internal error] term of class <" <<
                            copy->className() << "> not implemented for copying";
                    throw fl::Exception(ex.str());
                }
            }
            redraw();
        }

        void Term::onChangeToolBoxIndex(int index) {
            (void) index;
            delete dummyVariable->removeTerm(indexOfEditingTerm);
            switch (ui->tabTerms->currentIndex()) {
                case 0: dummyVariable->insertTerm(_basicTerms[ui->basicTermToolbox->currentIndex()]->copy(),
                            indexOfEditingTerm);
                    break;
                case 1:
                    dummyVariable->insertTerm(_extendedTerms[ui->extendedTermToolbox->currentIndex()]->copy(),
                            indexOfEditingTerm);
                    break;
                case 2:
                    dummyVariable->insertTerm(_edgeTerms[ui->edgeTermToolbox->currentIndex()]->copy(),
                            indexOfEditingTerm);
                    break;
                default:
                    throw fl::Exception("[internal error] index out of bounds");
            }
            redraw();
            this->adjustSize();
        }

        void Term::onChangeTab(int index) {
            setCurrentToolbox(index);
            onChangeToolBoxIndex(-1);
            redraw();
            this->adjustSize();
        }

        void Term::onChangeSpinBoxTriangle(double) {
            if (fl::Op::isGt(ui->sbx_triangle_a->value(), ui->sbx_triangle_b->value())) {
                ui->sbx_triangle_b->setValue(ui->sbx_triangle_a->value() + .1);
            }
            if (fl::Op::isGt(ui->sbx_triangle_b->value(), ui->sbx_triangle_c->value())) {
                ui->sbx_triangle_c->setValue(ui->sbx_triangle_b->value() + .1);
            }
            if (fl::Op::isGE(ui->sbx_triangle_a->value(), ui->sbx_triangle_c->value())) {
                ui->sbx_triangle_c->setValue(ui->sbx_triangle_a->value() + .1);
            }

            Triangle* term = dynamic_cast<Triangle*> (selectedTerm());
            term->setA(ui->sbx_triangle_a->value());
            term->setB(ui->sbx_triangle_b->value());
            term->setC(ui->sbx_triangle_c->value());

            redraw();
        }

        void Term::onChangeSpinBoxTrapezoid(double) {
            if (fl::Op::isGt(ui->sbx_trapezoid_a->value(), ui->sbx_trapezoid_b->value())) {
                ui->sbx_trapezoid_b->setValue(ui->sbx_trapezoid_a->value() + .1);
            }
            if (fl::Op::isGt(ui->sbx_trapezoid_b->value(), ui->sbx_trapezoid_c->value())) {
                ui->sbx_trapezoid_c->setValue(ui->sbx_trapezoid_b->value() + .1);
            }
            if (fl::Op::isGt(ui->sbx_trapezoid_c->value(), ui->sbx_trapezoid_d->value())) {
                ui->sbx_trapezoid_d->setValue(ui->sbx_trapezoid_c->value() + .1);
            }
            if (fl::Op::isGE(ui->sbx_trapezoid_a->value(), ui->sbx_trapezoid_d->value())) {
                ui->sbx_trapezoid_d->setValue(ui->sbx_trapezoid_a->value() + .1);
            }

            Trapezoid* term = dynamic_cast<Trapezoid*> (selectedTerm());
            term->setA(ui->sbx_trapezoid_a->value());
            term->setB(ui->sbx_trapezoid_b->value());
            term->setC(ui->sbx_trapezoid_c->value());
            term->setD(ui->sbx_trapezoid_d->value());
            redraw();
        }

        void Term::onChangeSpinBoxRectangle(double) {
            if (fl::Op::isGE(ui->sbx_rectangle_min->value(), ui->sbx_rectangle_max->value())) {
                ui->sbx_rectangle_max->setValue(ui->sbx_rectangle_min->value() + .1);
            }
            Rectangle* term = dynamic_cast<Rectangle*> (selectedTerm());
            term->setMinimum(ui->sbx_rectangle_min->value());
            term->setMaximum(ui->sbx_rectangle_max->value());
            redraw();
        }

        void Term::onClickDiscreteParser() {
            std::string xString = ui->led_discrete_x->text().toStdString();
            std::vector<std::string> xValues = fl::Op::split(xString, " ");

            std::string yString = ui->led_discrete_y->text().toStdString();
            std::vector<std::string> yValues = fl::Op::split(yString, " ");

            if (xValues.size() != yValues.size()) {
                QString xSize = QString::number(xValues.size());
                QString ySize = QString::number(yValues.size());

                QString message = QString("[Discrete] The lengths of vectors x and y differ: ");
                message += QString("x[") + xSize + QString("] and y[") + ySize + QString("]");
                QMessageBox::warning(this, tr("Warning"),
                        message, QMessageBox::Ok);
            }

            Discrete* term = dynamic_cast<Discrete*> (selectedTerm());
            term->x.clear();
            term->y.clear();

            int size = std::min(xValues.size(), yValues.size());
            try {
                for (int i = 0; i < size; ++i) {
                    FL_LOG("<" << xValues[i] << "," << yValues[i] << ">");
                    term->x.push_back(fl::Op::toScalar(xValues[i]));
                    term->y.push_back(fl::Op::toScalar(yValues[i]));
                }
            } catch (...) {
                QMessageBox::critical(this, tr("Error"),
                        "[Discrete] Vectors x and y must contain numeric values",
                        QMessageBox::Ok);
            }
            redraw();
        }

        void Term::onChangeSpinBoxGaussian(double) {
            Gaussian* term = dynamic_cast<Gaussian*> (selectedTerm());
            term->setMean(ui->sbx_gaussian_center->value());
            term->setSigma(ui->sbx_gaussian_width->value());
            redraw();
        }

        void Term::onChangeSpinBoxGaussianProduct(double) {
            GaussianProduct* term = dynamic_cast<GaussianProduct*> (selectedTerm());
            term->setMeanA(ui->sbx_gaussian_prod_center_a->value());
            term->setMeanB(ui->sbx_gaussian_prod_center_b->value());
            term->setSigmaA(ui->sbx_gaussian_prod_width_a->value());
            term->setSigmaB(ui->sbx_gaussian_prod_width_b->value());
            redraw();
        }

        void Term::onChangeSpinBoxBell(double) {
            Bell* term = dynamic_cast<Bell*> (selectedTerm());
            term->setCenter(ui->sbx_bell_center->value());
            term->setWidth(ui->sbx_bell_width->value());
            term->setSlope(ui->sbx_bell_slope->value());
            redraw();
        }

        void Term::onChangeSpinBoxPiShape(double) {
            PiShape* term = dynamic_cast<PiShape*> (selectedTerm());
            term->setA(ui->sbx_pishape_a->value());
            term->setB(ui->sbx_pishape_b->value());
            term->setC(ui->sbx_pishape_c->value());
            term->setD(ui->sbx_pishape_d->value());
            redraw();
        }

        void Term::onChangeSpinBoxSigmoidDiff(double) {
            SigmoidDifference* term = dynamic_cast<SigmoidDifference*> (selectedTerm());
            term->setFallingSlope(ui->sbx_sigmoid_diff_falling->value());
            term->setLeftInflection(ui->sbx_sigmoid_diff_left->value());
            term->setRightInflection(ui->sbx_sigmoid_diff_right->value());
            term->setRisingSlope(ui->sbx_sigmoid_diff_rising->value());
            redraw();
        }

        void Term::onChangeSpinBoxSigmoidProd(double) {
            SigmoidProduct* term = dynamic_cast<SigmoidProduct*> (selectedTerm());
            term->setFallingSlope(ui->sbx_sigmoid_prod_falling->value());
            term->setLeftInflection(ui->sbx_sigmoid_prod_left->value());
            term->setRightInflection(ui->sbx_sigmoid_prod_right->value());
            term->setRisingSlope(ui->sbx_sigmoid_prod_rising->value());
            redraw();
        }

        //EDGES

        void Term::onChangeSpinBoxRamp(double) {
            Ramp* term = dynamic_cast<Ramp*> (selectedTerm());
            term->setStart(ui->sbx_ramp_start->value());
            term->setEnd(ui->sbx_ramp_end->value());
            redraw();
        }

        void Term::onChangeSpinBoxSigmoid(double) {
            Sigmoid* term = dynamic_cast<Sigmoid*> (selectedTerm());
            term->setInflection(ui->sbx_sigmoid_inflection->value());
            term->setSlope(ui->sbx_sigmoid_slope->value());
            redraw();
        }

        void Term::onChangeSpinBoxSShape(double) {
            if (fl::Op::isGE(ui->sbx_sshape_start->value(), ui->sbx_sshape_end->value())) {
                ui->sbx_sshape_end->setValue(ui->sbx_sshape_start->value() + .1);
            }
            SShape* term = dynamic_cast<SShape*> (selectedTerm());
            term->setStart(ui->sbx_sshape_start->value());
            term->setEnd(ui->sbx_sshape_end->value());
            redraw();
        }

        void Term::onChangeSpinBoxZShape(double) {
            if (fl::Op::isGE(ui->sbx_zshape_start->value(), ui->sbx_zshape_end->value())) {
                ui->sbx_zshape_end->setValue(ui->sbx_zshape_start->value() + .1);
            }
            ZShape* term = dynamic_cast<ZShape*> (selectedTerm());
            term->setStart(ui->sbx_zshape_start->value());
            term->setEnd(ui->sbx_zshape_end->value());
            redraw();
        }

        void Term::loadFrom(const fl::Term* x) {
            //BASIC
            if (x->className() == Triangle().className()) {
                const Triangle* term = dynamic_cast<const Triangle*> (x);
                ui->sbx_triangle_a->setValue(term->getA());
                ui->sbx_triangle_b->setValue(term->getB());
                ui->sbx_triangle_c->setValue(term->getC());
                ui->basicTermToolbox->setCurrentIndex(0);
                setCurrentToolbox(0);

            } else if (x->className() == Trapezoid().className()) {
                const Trapezoid* term = dynamic_cast<const Trapezoid*> (x);
                ui->sbx_trapezoid_a->setValue(term->getA());
                ui->sbx_trapezoid_b->setValue(term->getB());
                ui->sbx_trapezoid_c->setValue(term->getC());
                ui->sbx_trapezoid_d->setValue(term->getD());
                ui->basicTermToolbox->setCurrentIndex(1);
                setCurrentToolbox(0);

            } else if (x->className() == Rectangle().className()) {
                const Rectangle* term = dynamic_cast<const Rectangle*> (x);
                ui->sbx_rectangle_min->setValue(term->getMinimum());
                ui->sbx_rectangle_max->setValue(term->getMaximum());
                ui->basicTermToolbox->setCurrentIndex(2);
                setCurrentToolbox(0);


            } else if (x->className() == Discrete().className()) {
                const Discrete* term = dynamic_cast<const Discrete*> (x);
                std::ostringstream ssX, ssY;
                int size = std::min(term->x.size(), term->y.size());
                for (int i = 0; i < size; ++i) {
                    ssX << term->x[i];
                    ssY << term->y[i];
                    if (i < size - 1) {
                        ssX << " ";
                        ssY << " ";
                    }
                }
                ui->led_discrete_x->setText(QString::fromStdString(ssX.str()));
                ui->led_discrete_y->setText(QString::fromStdString(ssY.str()));
                ui->basicTermToolbox->setCurrentIndex(3);
                setCurrentToolbox(0);

                //EXTENDED
            } else if (x->className() == Gaussian().className()) {
                const Gaussian* term = dynamic_cast<const Gaussian*> (x);
                ui->sbx_gaussian_center->setValue(term->getMean());
                ui->sbx_gaussian_width->setValue(term->getSigma());
                ui->extendedTermToolbox->setCurrentIndex(0);
                setCurrentToolbox(1);

            } else if (x->className() == GaussianProduct().className()) {
                const GaussianProduct* term = dynamic_cast<const GaussianProduct*> (x);
                ui->sbx_gaussian_prod_center_a->setValue(term->getMeanA());
                ui->sbx_gaussian_prod_center_b->setValue(term->getMeanB());
                ui->sbx_gaussian_prod_width_a->setValue(term->getSigmaA());
                ui->sbx_gaussian_prod_width_b->setValue(term->getSigmaB());
                ui->extendedTermToolbox->setCurrentIndex(1);
                setCurrentToolbox(1);

            } else if (x->className() == Bell().className()) {
                const Bell* term = dynamic_cast<const Bell*> (x);
                ui->sbx_bell_center->setValue(term->getCenter());
                ui->sbx_bell_width->setValue(term->getWidth());
                ui->sbx_bell_slope->setValue(term->getSlope());
                ui->extendedTermToolbox->setCurrentIndex(2);
                setCurrentToolbox(1);

            } else if (x->className() == PiShape().className()) {
                const PiShape* term = dynamic_cast<const PiShape*> (x);
                ui->sbx_pishape_a->setValue(term->getA());
                ui->sbx_pishape_b->setValue(term->getB());
                ui->sbx_pishape_c->setValue(term->getC());
                ui->sbx_pishape_d->setValue(term->getD());
                ui->extendedTermToolbox->setCurrentIndex(3);
                setCurrentToolbox(1);

            } else if (x->className() == SigmoidDifference().className()) {
                const SigmoidDifference* term = dynamic_cast<const SigmoidDifference*> (x);
                ui->sbx_sigmoid_diff_falling->setValue(term->getFallingSlope());
                ui->sbx_sigmoid_diff_left->setValue(term->getLeftInflection());
                ui->sbx_sigmoid_diff_right->setValue(term->getRightInflection());
                ui->sbx_sigmoid_diff_rising->setValue(term->getRisingSlope());
                ui->extendedTermToolbox->setCurrentIndex(4);
                setCurrentToolbox(1);

            } else if (x->className() == SigmoidProduct().className()) {
                const SigmoidProduct* term = dynamic_cast<const SigmoidProduct*> (x);
                ui->sbx_sigmoid_prod_falling->setValue(term->getFallingSlope());
                ui->sbx_sigmoid_prod_left->setValue(term->getLeftInflection());
                ui->sbx_sigmoid_prod_right->setValue(term->getRightInflection());
                ui->sbx_sigmoid_prod_rising->setValue(term->getRisingSlope());
                ui->extendedTermToolbox->setCurrentIndex(5);
                setCurrentToolbox(1);

                //EDGE
            } else if (x->className() == Ramp().className()) {
                const Ramp* term = dynamic_cast<const Ramp*> (x);
                ui->sbx_ramp_start->setValue(term->getStart());
                ui->sbx_ramp_end->setValue(term->getEnd());
                ui->edgeTermToolbox->setCurrentIndex(0);
                setCurrentToolbox(2);

            } else if (x->className() == Sigmoid().className()) {
                const Sigmoid* term = dynamic_cast<const Sigmoid*> (x);
                ui->sbx_sigmoid_inflection->setValue(term->getInflection());
                ui->sbx_sigmoid_slope->setValue(term->getSlope());
                ui->edgeTermToolbox->setCurrentIndex(1);
                setCurrentToolbox(2);

            } else if (x->className() == SShape().className()) {
                const SShape* term = dynamic_cast<const SShape*> (x);
                ui->sbx_sshape_start->setValue(term->getStart());
                ui->sbx_sshape_end->setValue(term->getEnd());
                ui->edgeTermToolbox->setCurrentIndex(2);
                setCurrentToolbox(2);

            } else if (x->className() == ZShape().className()) {
                const ZShape* term = dynamic_cast<const ZShape*> (x);
                ui->sbx_zshape_start->setValue(term->getStart());
                ui->sbx_zshape_end->setValue(term->getEnd());
                ui->edgeTermToolbox->setCurrentIndex(3);
                setCurrentToolbox(2);
            } else {
                std::ostringstream ex;
                ex << "[internal error] Term class <" << x->className() << "> not registered";
                throw fl::Exception(ex.str());
            }
            redraw();
        }

        void Term::main() {
            Term* t = new Term;
            t->setup(fl::Variable("", 0, 1));
            t->show();
        }

    }
}