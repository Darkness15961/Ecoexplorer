document.addEventListener('DOMContentLoaded', function () {
  const dias = ['Lunes', 'Martes', 'Miércoles', 'Jueves', 'Viernes', 'Sábado', 'Domingo'];
  const chartInstances = {};

  const ctxMap = {
    'Temperatura': document.getElementById('chartTemperatura').getContext('2d'),
    'Humedad': document.getElementById('chartHumedad').getContext('2d'),
    'Gas': document.getElementById('chartGas').getContext('2d'),
    'IndiceUV': document.getElementById('chartIndiceUV').getContext('2d'),
    'Presion': document.getElementById('chartPresion').getContext('2d'),
    'Sonido': document.getElementById('chartSonido').getContext('2d'),
  };

  function generateLabels(range) {
    return range === 'Hoy'
      ? Array.from({ length: 24 }, (_, i) => `${i}:00`)
      : dias;
  }

  async function fetchData(range, tipo) {
    try {
      const res = await fetch(`https://ecox.blocmin.com/api/get-data-format?tipo=${tipo}&rango=${range}`);
      const data = await res.json();
      return data;
    } catch (e) {
      console.error("Error fetching data:", e);
      return [];
    }
  }

  async function updateChart(tipo, range) {
    const dataAPI = await fetchData(range, tipo);
    const labels = generateLabels(range);
    const dataPoints = dataAPI.map(item => item.promedio);

    const data = {
      labels: labels,
      datasets: [{
        label: tipo,
        data: dataPoints,
        borderColor: '#3b82f6',
        backgroundColor: '#3b82f6',
        fill: false,
        tension: 0.4
      }]
    };

    const options = {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        y: {
          beginAtZero: true,
          grid: { display: false }
        },
        x: {
          grid: { display: true }
        }
      }
    };

    if (chartInstances[tipo]) {
      chartInstances[tipo].destroy();
    }

    chartInstances[tipo] = new Chart(ctxMap[tipo], {
      type: 'line',
      data: data,
      options: options
    });
  }

  // Manejar eventos de cada botón
  const rangeButtons = document.querySelectorAll('.btn-range');
  rangeButtons.forEach(btn => {
    btn.addEventListener('click', () => {
      const tipo = btn.getAttribute('data-tipo');
      const rango = btn.getAttribute('data-rango');

      // Remueve clase active de otros botones del mismo tipo
      const siblings = document.querySelectorAll(`.btn-range[data-tipo="${tipo}"]`);
      siblings.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');

      updateChart(tipo, rango);
    });
  });

  // Cargar TODAS las gráficas en "Hoy" al inicio
  Object.keys(ctxMap).forEach(tipo => updateChart(tipo, 'Hoy'));
});
