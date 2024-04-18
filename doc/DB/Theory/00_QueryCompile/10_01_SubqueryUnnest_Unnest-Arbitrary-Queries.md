定义dependent join
$$
\begin{eqnarray}
T_1 \ dj \ T_2 \coloneq \left \{ t_1 \circ t_2  \right | t_1 \in T_1 \wedge  t_2 \in T_2(t_1) \wedge p(t_1 \circ t_2) \} 
\end{eqnarray}
$$

首先，将一个dependent join转化为一个更优的dependent join，有下面的等价变换：
$$
\begin{array}{l}
T_1 \ dj \ T_2 \equiv T_1 \Join_{p \wedge T_1 =\mathcal{A}(D)}^D(D \ dj \ T_2) \\
其中:D \coloneq \Pi_{\mathcal{F}(T_2) \cap \mathcal{A}(T_1)}(T_1)
\end{array}
$$

$$
\begin{array}{l}
D \ dj \ T \equiv D \Join T & if \ \mathcal{F}(T) \cap \mathcal{A}(D) =  \emptyset
\end{array}
$$

$$
\begin{array}{l}
D \ dj \ \sigma_p(T_2) \equiv \sigma_p(D \ dj \ T_2)\\
\end{array}
$$