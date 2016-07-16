#include "dojosynapse.h"
#include <QDateTime>
#include "math.h"

/*!
    \class dojoSynapse
    \brief The dojoSynapse class provides API to change synaptic data.

    dojoSynapse is an implementation of neuron connection influenced by real synapses.

    It defines cleft and post-synaptic characteristics : voltage, post-synaptic permeability changes over the time.
    Pre-synaptic characteristics are implemented in dojoNeuron class.

    Synaptic plasticity is described by long term (LTP) and short term (STP).

    \p LTP

    LTP is implemented in changePermeability() and it changes synaptic permability upon generated spikes of post synaptic neuron
    In biology it reflects number of Ca2+ receptors (NMDA and AMPA).

    (wiki)
    Brief activation of an excitatory pathway can produce long-term depression (LTD).
    LTD is induced by a minimum level of postsynaptic depolarization and simultaneous increase in the intracellular calcium concentration at the postsynaptic neuron.
    LTD can be initiated at inactive synapses if the calcium concentration is raised to the minimum required level by heterosynaptic activation, or if the extracellular concentration is raised.
    LTD can be induced by 1 Hz in 10 minutes.
    EPSP is decresing down in 1.5-2 times.

    LTP is an increase in synaptic response following potentiating pulses of electrical stimuli that sustains at a level above the baseline response for hours or longer.
    LTP involves interactions between postsynaptic neurons and the specific presynaptic inputs that form a synaptic association, and is specific to the stimulated pathway of synaptic transmission.
    The long-term stabilization of synaptic changes is determined by a parallel increase of pre- and postsynaptic structures such as axonal bouton, dendritic spine and postsynaptic density.
    LTP can be induced by 100 Hz in 1 sec per 4 times. EPSP is growing up in 2 times.

    \p STP
    STP is implemented in dojoNeuron class as it reflects probability of probability of synaptic terminals releasing transmitters.
    That is done by dojoNeuron.

    Depending on the time scales over which it acts synaptic enhancement is classified as neural facilitation, synaptic augmentation or post-tetanic potentiation.

    Also STP can be seen in STDP (spike time dependent placticity.
    It means if postsynaptic cell is firing in 10-20 after spike then this synapse matters and EPSP is growing up.
    If postsynaptic cell is firing in 100-50 ms before spike then this synapse doesn't matter and EPSP is reducing down.


 */
/*!
 * \brief dojoSynapse::dojoSynapse
 * \param syn
 * \param str
 */
dojoSynapse::dojoSynapse(QString syn, dojoStorage *str){
    synapse = syn;
    storage = str;

    cleft = 0;
    lastSpike = QDateTime::currentMSecsSinceEpoch();
    neuronSpikeTime = lastSpike+1;

    qDebug()<<"ls: "<<lastSpike;
}
/*!
 * \brief dojoSynapse::getVoltage
 * \param now
 * \return
 * The function is called by destanation neuron whenever it collects all synapses changes.
 * It returns voltage value which is calculated on current timestamps since last spike, cleft mediator value and permeability which reflects history of the synapse actions.
 * Current timestamp should be provided by calling neuron.
 */
float dojoSynapse::getVoltage(){
    //calc time-depend value
    float binded = 1-exp(-(QDateTime::currentMSecsSinceEpoch()  - lastSpike)/TIME_CONST);

    //binded mediator
    float voltage = cleft*binded;

    //remove binded mediator from cleft
    cleft -= voltage;

    //membrane volatge change depends on synapse permability
    float vlt = voltage*storage->getSynapsePermability(synapse);


    //volatge change value also depends on distanse of this synapse from the soma
    vlt = vlt*exp(-storage->getSynapseLength(synapse)/LENGTH_CONST);
    //DEBUG
    //qDebug()<<now<<" : "<<synapse<<'-'<<vlt;
    //return voltage which depends on length of this synapse and its permability
    return vlt;
}
/*!
 * \brief dojoSynapse::handleSpike
 * \param transmiterValue
 * It is calling by source neuron when spike event catched.
 * It release mediator into the cleft from source terminals that value is provided by calling neuron.
 * It saves current timestamp to calculate time depending characteristics in further use in dojoSynapse::getVoltage() function.
 *
 * Also this function implements "learning" at the neuron's spike event.
 * In general it descibes the "Hebb" rule but we implemented it as STDP
 */
void dojoSynapse::handleSpike(float transmiterValue, qint64 time){
    //save moment of this action
    lastSpike = time;
    //release mediator from terminals to cleft
    cleft += transmiterValue;

    //STDP to increase perm
    qint64 spikeLag = neuronSpikeTime-time;

    float currentPermability = storage->getSynapsePermability(synapse);

    float newperm = currentPermability-LEARNING_RATE*currentPermability*exp(spikeLag/10);
    //qDebug()<<synapse<<" - cp: "<<currentPermability<<", pre: "<<spikeLag<<", np: "<<newperm;

    storage->setSynapsePermability(synapse, newperm);

}
void dojoSynapse::postNeuronSpike(qint64 spikeTime){
    //STDP to decrease perm
    qint64 spikeLag = spikeTime-lastSpike;
    float currentPermability = storage->getSynapsePermability(synapse);
    float newperm =  currentPermability+LEARNING_RATE*currentPermability*exp(-spikeLag/20);
    //qDebug()<<synapse<<" - cp: "<<currentPermability<<", post: "<<spikeLag<<", np: "<<newperm;

    storage->setSynapsePermability(synapse,newperm);

   //save time of this spike
   neuronSpikeTime =  spikeTime;
}
