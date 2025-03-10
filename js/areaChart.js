// Definir variables globales para colores
const COLORS = {
    primary: "rgba(69, 170, 150, 1)",
    background: "rgba(255,255,255)",
    grid: "rgb(234, 236, 244)",
    tooltipBg: "rgb(69, 170, 150)",
    tooltipBorder: "#02eae3",
    tooltipTitle: "#2ecc71 ",
    tooltipText: "#e74c3c"
};

// Función para formatear números con separadores de miles y decimales
function numberFormat(number, decimals = 0, decPoint = '.', thousandsSep = ',') {
    number = (number + '').replace(',', '').replace(' ', '');
    let n = !isFinite(+number) ? 0 : +number,
        prec = !isFinite(+decimals) ? 0 : Math.abs(decimals),
        s = '',
        toFixedFix = (n, prec) => {
            let k = Math.pow(10, prec);
            return '' + Math.round(n * k) / k;
        };
    s = (prec ? toFixedFix(n, prec) : '' + Math.round(n)).split('.');
    if (s[0].length > 3) {
        s[0] = s[0].replace(/\B(?=(?:\d{3})+(?!\d))/g, thousandsSep);
    }
    if ((s[1] || '').length < prec) {
        s[1] = s[1] || '';
        s[1] += new Array(prec - s[1].length + 1).join('0');
    }
    return s.join(decPoint);
}

// Esperar a que el DOM cargue completamente antes de ejecutar el código
document.addEventListener("DOMContentLoaded", function () {
    const ctx = document.getElementById("myAreaChart");
    if (!ctx) {
        console.error("Elemento con ID 'myAreaChart' no encontrado.");
        return;
    }

    new Chart(ctx, {
        type: 'line',
        data: {
            labels: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"],
            datasets: [{
                label: "Earnings",
                tension: 0.3,
                backgroundColor: COLORS.background,
                borderColor: COLORS.primary,
                pointRadius: 3,
                pointBackgroundColor: COLORS.primary,
                pointBorderColor: COLORS.primary,
                pointHoverRadius: 5,
                pointHoverBackgroundColor: COLORS.primary,
                pointHoverBorderColor: COLORS.primary,
                pointHitRadius: 10,
                pointBorderWidth: 2,
                data: [0, 10000, 5000, 15000, 10000, 20000, 15000, 25000, 20000, 30000, 25000, 40000]
            }]
        },
        options: {
            maintainAspectRatio: false,
            layout: {
                padding: { left: 10, right: 25, top: 25, bottom: 0 }
            },
            scales: {
                x: {
                    grid: { display: false, drawBorder: false },
                    ticks: { maxTicksLimit: 7 }
                },
                y: {
                    ticks: {
                        maxTicksLimit: 5,
                        padding: 10,
                        callback: value => '$' + numberFormat(value)
                    },
                    grid: {
                        color: COLORS.grid,
                        drawBorder: false,
                        borderDash: [2]
                    }
                }
            },
            plugins: {
                legend: { display: false },
                tooltip: {
                    backgroundColor: COLORS.tooltipBg,
                    bodyColor: COLORS.tooltipText,
                    titleMarginBottom: 10,
                    titleColor: COLORS.tooltipTitle,
                    titleFont: { size: 14 },
                    borderColor: COLORS.tooltipBorder,
                    borderWidth: 1,
                    padding: 15,
                    displayColors: false,
                    intersect: false,
                    mode: 'index',
                    caretPadding: 10,
                    callbacks: {
                        label: tooltipItem => `${tooltipItem.dataset.label}: $${numberFormat(tooltipItem.raw)}`
                    }
                }
            }
        }
    });
});