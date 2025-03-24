struct intpair {
        int a;
        int b;
};

program CALCULATOR {
        version CALCULATOR_VERS {
                int ADDITION(intpair) = 1;
		int SUBTRACTION(intpair) =2;
		int MULTIPLICATION(intpair) = 3;
		double DIVISION(intpair) =4;
        } = 1;
} = 0x23451111;
