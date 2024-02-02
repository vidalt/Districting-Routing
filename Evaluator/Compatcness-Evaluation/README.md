# Compactness Evaluation

Code to evaluate compactness of a given solution with the formula:

$$ \frac{A}{\pi r^2_{enc}} $$

$A$ holds for the area of a region, $r_{enc}$ is the ratio of the smallest circle that contains that region. The general idea is that compact areas are similar to a circle. 

To compile code just run `` make ``, it is necessary to have g++ installed. To execute run:

```
./compactness-evaluation <path-to-solution>
```

The input solutions format is described [here](https://github.com/vidalt/Districting/tree/master/Solver/ILS).
The output format is the compactness of the each district separated by line break, it created a file in `` <path-to-solution>.eval``.
