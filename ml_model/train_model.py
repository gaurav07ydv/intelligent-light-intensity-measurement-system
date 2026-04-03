import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from xgboost import XGBRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_absolute_error, r2_score

pd.set_option("display.max_columns", None)

print("Loading dataset...")

# Load new dataset
data = pd.read_csv("Lux_data.csv", encoding="latin1", on_bad_lines="skip")

# Clean column names
data.columns = data.columns.str.strip()

print("Columns found:", data.columns.tolist())

# Convert to numeric
for col in data.columns:
    data[col] = pd.to_numeric(data[col], errors="coerce")

# Drop invalid rows
data = data.dropna()

print("Clean dataset size:", data.shape)

# Required columns
required_cols = ["ldr", "temp", "tsl2561", "bh1750"]
for col in required_cols:
    if col not in data.columns:
        raise ValueError(f"Missing column: {col}")

# Inputs and target
X = data[["ldr", "temp"]]
y = data["tsl2561"]

print("Features:", X.columns.tolist())
print("Target:", y.name)

# Train-test split
X_train, X_test, y_train, y_test = train_test_split(
    X, y,
    test_size=0.2,
    random_state=42
)

# BH1750 values for validation
bh_test = data.loc[X_test.index, "bh1750"]

# XGBoost model
model = XGBRegressor(
    n_estimators=200,
    learning_rate=0.05,
    max_depth=4,
    subsample=0.8,
    colsample_bytree=0.8,
    random_state=42
)

# Train model
model.fit(X_train, y_train)
print("Model trained")

# Predict
pred = model.predict(X_test)

# Clamp negative lux values to 0
pred = np.maximum(pred, 0)

# Performance
mae = mean_absolute_error(y_test, pred)
r2 = r2_score(y_test, pred)

print("\nPerformance:")
print("MAE:", mae)
print("R2:", r2)

# Final output table
results = pd.DataFrame({
    "ldr": X_test["ldr"].values,
    "temp": X_test["temp"].values,
    "tsl2561": y_test.values,
    "predicted_lux": pred,
    "bh1750": bh_test.values
})

# Add one error column
results["error"] = np.abs(results["tsl2561"] - results["predicted_lux"])

results = results.reset_index(drop=True)

print("\nFinal Output (Sample):")
print(results.head(20))

# Save final output
results.to_csv("final_simple_output.csv", index=False)
print("\nSaved: final_simple_output.csv")

# -------------------------
# GRAPHS
# -------------------------

# 1. Predicted vs TSL2561
plt.figure(figsize=(8, 6))
plt.scatter(results["tsl2561"], results["predicted_lux"],
            alpha=0.6, label="Predicted Data")

plt.plot(
    [results["tsl2561"].min(), results["tsl2561"].max()],
    [results["tsl2561"].min(), results["tsl2561"].max()],
    linestyle="--",
    label="Ideal Fit (y = x)"
)

plt.xlabel("Reference Lux (TSL2561)")
plt.ylabel("Predicted Lux")
plt.title("Model Calibration: Predicted vs Reference")
plt.legend()
plt.axis("equal")
plt.grid(True)
plt.tight_layout()
plt.show()

# 2. Predicted vs BH1750
plt.figure(figsize=(8, 6))
plt.scatter(results["bh1750"], results["predicted_lux"],
            alpha=0.6, label="Predicted Data")

plt.plot(
    [results["bh1750"].min(), results["bh1750"].max()],
    [results["bh1750"].min(), results["bh1750"].max()],
    linestyle="--",
    label="Ideal Fit (y = x)"
)

plt.xlabel("BH1750 Lux")
plt.ylabel("Predicted Lux")
plt.title("Validation: Predicted vs BH1750")
plt.legend()
plt.axis("equal")
plt.grid(True)
plt.tight_layout()
plt.show()

# 3. TSL2561 vs BH1750
plt.figure(figsize=(8, 6))
plt.scatter(results["tsl2561"], results["bh1750"],
            alpha=0.6, label="Sensor Data")

plt.plot(
    [results["tsl2561"].min(), results["tsl2561"].max()],
    [results["tsl2561"].min(), results["tsl2561"].max()],
    linestyle="--",
    label="Ideal Match (y = x)"
)

plt.xlabel("TSL2561 (Reference)")
plt.ylabel("BH1750")
plt.title("Sensor Comparison: TSL2561 vs BH1750")
plt.legend()
plt.axis("equal")
plt.grid(True)
plt.tight_layout()
plt.show()

# 4. Feature Importance
importances = model.feature_importances_
features = ["ldr", "temp"]

plt.figure(figsize=(8, 6))
plt.bar(features, importances)

plt.xlabel("Features")
plt.ylabel("Importance Score")
plt.title("Feature Importance (XGBoost Model)")

plt.grid(True)
plt.tight_layout()
plt.show()

print("\nFeature Importance:")
for f, imp in zip(features, importances):
    print(f"{f}: {imp:.4f}")

