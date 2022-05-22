#ifndef CONCURRENT_SERVER_ASSEM_HPP
#define CONCURRENT_SERVER_ASSEM_HPP
#include "Assembler.hpp"
#include "GoldbachCalculator.hpp"

template<typename ConsumerType, typename ProducerType>
class GoldbachAssembler : public Assembler<ConsumerType, ProducerType>{
public:
	/**
	 * @brief Constructor para GoldbachAssembler
	 * */
	explicit GoldbachAssembler(Queue<ConsumerType>* consumingQueue = nullptr,
		Queue<ProducerType>* producingQueue = nullptr,
		const ConsumerType& stopCondition = ConsumerType()):

	Assembler<ConsumerType, ProducerType>(consumingQueue, producingQueue,
		stopCondition) {}

	virtual ~GoldbachAssembler() {};

private:
	GoldbachCalculator calculator;

	/**
	 * @brief Override de la función 'run' en la clase Thread.
	 * @return 0 If success.
	 **/
	virtual int run() {
		this->consumeForever();
		return 0;
	}

	/**
	 * @brief Override de función 'consume' en clase Consumer.
	 * @param Template data to consume (Pointer to SubList).
	 **/
	virtual void consume(const ConsumerType& data) {
		calculator.calculateGoldbach(data);
		assemblerProduce(data);
		deliver();
	}

	/**
	 * @brief Si todos los elementos del padre SuperList se han procesado, envía
	 * una señal a WebServer para continuar con el MessageResponse para Socket.
	 * @param data Puntero a SubList que contiene todas las sumas.
	 **/
	virtual void deliver() {
		ConsumerType data = this->getProducingQueue()->pop();
		auto* parent = reinterpret_cast<SuperList*>(data->getParent());

		if (parent != nullptr) {
			parent->increaseCompleted();

			if (parent->getCompleted() == parent->getSuperListLength()) {
				parent->signalCompletion();
			}
		}
	}

	/**
	 * @brief Llama al método produce() de la clase Producer.
	 * @param data Datos a meter en la Producing Queue.
	 * */
	void assemblerProduce(const ProducerType& data) {
		this->produce(data);
	}
};
#endif //CONCURRENT_SERVER_ASSEM_HPP
