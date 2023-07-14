交换律
$$
\begin{array}{l}
A \times B &=& B \times A \\
A \Join B &=& B \Join A \\
A \Join_F B &=& B \Join_F A && 其中F是约束条件\\
\Pi_P(\sigma_F(B)) &=& \sigma_F(\Pi_p(B)) &&其中F \in p \\
\end{array}
$$
结合律
$$
\begin{array}{l}
(A \times B) \times C &=& A \times (B \times C) \\
(A \Join B) \Join C &=& A \times (B \Join C) \\
(A \Join_{F_1} B) \Join_{F_2} C &=& A \Join_{F_1} (B \Join_{F_2} C) && 其中F_1和F_2是约束条件\\
\end{array}
$$
分配律
$$
\begin{array}{l}
\sigma_F(A \times B) &=& \sigma_F(A) \times B &&其中F \in A \\
\sigma_F(A \times B) &=& \sigma_{F_1}(A) \times \sigma_{F_2}(B) &&其中F=F_1 \cup F_2,F_1 \in A, F_2 \in B  \\
\sigma_F(A \times B) &=& \sigma_{F_x}(\sigma_{F_1}(A) \times \sigma_{F_2}(B)) &&其中F=F_1 \cup F_2 \cup F_x, F_1\in A, F_2 \in B  \\
\Pi_{p, q}(A \times B) &=& \Pi(A) \times \Pi_q(B) && 其中p \in A, q \in B \\
\sigma_F(A \times B) &=& \sigma_{F_1}(A) \times \sigma_{F_2}(B) && 其中F=F_1 \cup F_2,F_1 \in A, F_2 \in B  \\
\end{array}
$$
串接律
$$
\begin{array}{l}
\Pi_{P=p_1, p_2,..., p_n}(\Pi_{Q=q_1, q_2,..., q_n(A)}) &=& \Pi_(P=p_1, p_2,.., p_n)(A)&& 其中P \subseteq Q \\
\sigma_{F_1}(\sigma_{F_2}(A)) &=& \sigma_{F_1 \cap F_2}(A) \\
\end{array}
$$
